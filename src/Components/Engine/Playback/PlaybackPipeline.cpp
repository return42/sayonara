/* PlaybackPipeline.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlaybackPipeline.h"
#include "PipelineProbes.h"
#include "SpeedHandler.h"
#include "EqualizerHandler.h"
#include "SeekHandler.h"
#include "StreamRecorderHandler.h"
#include "StreamRecorderData.h"

#include "Components/Engine/Callbacks/PipelineCallbacks.h"
#include "Utils/globals.h"
#include "Utils/Utils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <gst/base/gstdataqueue.h>
#include <gst/app/gstappsink.h>

#include <algorithm>

//http://gstreamer.freedesktop.org/data/doc/gstreamer/head/manual/html/chapter-dataaccess.html

struct PlaybackPipeline::Private :
		public SpeedHandler,
		public EqualizerHandler,
		public StreamRecorderHandler,
		public SeekHandler
{
	GstElement*			audio_src=nullptr;
	GstElement*			audio_convert=nullptr;
	GstElement*			tee=nullptr;

	GstElement*			eq_queue=nullptr;
	GstElement*			equalizer=nullptr;
	GstElement*			speed=nullptr;
	GstElement*			volume=nullptr;
	GstElement*			pitch=nullptr;

	GstElement*			audio_sink=nullptr;

	GstElement*			spectrum_queue=nullptr;
	GstElement*			spectrum=nullptr;
	GstElement*			spectrum_sink=nullptr;

	GstElement*			level_queue=nullptr;
	GstElement*			level=nullptr;
	GstElement*			level_sink=nullptr;

	GstElement*			lame_queue=nullptr;
	GstElement*			lame_converter=nullptr;
	GstElement*			lame_resampler=nullptr;
	GstElement*			lame=nullptr;
	GstElement*			lame_app_sink=nullptr;

	GstElement*			file_queue=nullptr;
	GstElement*			file_converter=nullptr;
	GstElement*			file_sink=nullptr;
	GstElement*			file_resampler=nullptr;
	GstElement*			file_lame=nullptr;

	gulong				level_probe, spectrum_probe, lame_probe, file_probe;
	int					vol;
	bool				show_level, show_spectrum, run_broadcast, run_sr;

	Private() :
		SpeedHandler(),
		EqualizerHandler(),
		StreamRecorderHandler(),
		SeekHandler(),

		level_probe(0),
		spectrum_probe(0),
		lame_probe(0),
		file_probe(0),
		vol(0),
		show_level(false),
		show_spectrum(false),
		run_broadcast(false),
		run_sr(false)
	{}


	protected:
		GstElement* get_pitch_element() const override
		{
			return pitch;
		}

		GstElement* get_equalizer_element() const override
		{
			return equalizer;
		}

		GstElement* get_streamrecorder_sink_element() const override
		{
			return file_sink;
		}

		GstElement* get_source() const override
		{
			return audio_src;
		}
};


PlaybackPipeline::PlaybackPipeline(Engine* engine, QObject *parent) :
	AbstractPipeline("Playback Pipeline", engine, parent),
	CrossFader(),
	ChangeablePipeline()
{
	m = Pimpl::make<Private>();
}

PlaybackPipeline::~PlaybackPipeline() {}

bool PlaybackPipeline::init(GstState state)
{
	if(!AbstractPipeline::init(state)){
		return false;
	}

	_settings->set(SetNoDB::MP3enc_found, m->lame != nullptr);
	_settings->set(SetNoDB::Pitch_found, m->lame != nullptr);

    Set::listen(Set::Engine_Vol, this, &PlaybackPipeline::s_vol_changed);
    Set::listen(Set::Engine_Mute, this, &PlaybackPipeline::s_mute_changed);

	// set by gui, initialized directly in pipeline
    Set::listen(Set::Engine_ShowLevel, this, &PlaybackPipeline::s_show_level_changed);
    Set::listen(Set::Engine_ShowSpectrum, this, &PlaybackPipeline::s_show_spectrum_changed);
    Set::listen(Set::Engine_Pitch, this, &PlaybackPipeline::s_speed_changed);
    Set::listen(Set::Engine_Speed, this, &PlaybackPipeline::s_speed_changed);
    Set::listen(Set::Engine_PreservePitch, this, &PlaybackPipeline::s_speed_changed);
    Set::listen(Set::Engine_SpeedActive, this, &PlaybackPipeline::s_speed_active_changed);

	set_n_sound_receiver(false);

	set_streamrecorder_path("");

	return true;
}

bool PlaybackPipeline::create_elements()
{
	// input
	if(!create_element(&m->audio_src, "uridecodebin", "src")) return false;
	if(!create_element(&m->audio_convert, "audioconvert")) return false;
	if(!create_element(&m->equalizer, "equalizer-10bands")) return false;

	if(!create_element(&m->pitch, "pitch")){
		m->pitch = nullptr;
	}

	if(!create_element(&m->tee, "tee")) return false;

	// standard output branch
	if(!create_element(&m->eq_queue, "queue", "eq_queue")) return false;
	if(!create_element(&m->volume, "volume")) return false;
	if(!create_element(&m->audio_sink, "autoaudiosink")) return false;

	// level branch
	if(!create_element(&m->level_queue, "queue", "level_queue")) return false;
	if(!create_element(&m->level, "level")) return false;
	if(!create_element(&m->level_sink, "fakesink", "level_sink")) return false;

	// spectrum branch
	if(!create_element(&m->spectrum_queue, "queue", "spectrum_queue")) return false;
	if(!create_element(&m->spectrum, "spectrum")) return false;
	if(!create_element(&m->spectrum_sink,"fakesink", "spectrum_sink")) return false;

	// lame branch
	if( !create_element(&m->lame_queue, "queue", "lame_queue") ||
		!create_element(&m->lame_converter, "audioconvert", "lame_converter") ||
		!create_element(&m->lame_resampler, "audioresample", "lame_resampler") ||
		!create_element(&m->lame, "lamemp3enc") ||
		!create_element(&m->lame_app_sink, "appsink", "lame_appsink"))
	{
		m->lame = nullptr;
	}

	// stream recorder branch
	if(	!create_element(&m->file_queue, "queue", "sr_queue") ||
		!create_element(&m->file_converter, "audioconvert", "sr_converter") ||
		!create_element(&m->file_resampler, "audioresample", "sr_resample") ||
		!create_element(&m->file_lame, "lamemp3enc", "sr_lame")  ||
		!create_element(&m->file_sink, "filesink", "sr_filesink"))
	{
		m->file_sink = nullptr;
	}

	else{
		m->streamrecorder_data()->queue = m->file_queue;
		m->streamrecorder_data()->sink = m->file_sink;
    }


	return true;
}

bool PlaybackPipeline::add_and_link_elements()
{
	bool success;
	GstPadTemplate* tee_src_pad_template;

	gst_bin_add_many(GST_BIN(_pipeline),
					 m->audio_src, m->audio_convert, m->equalizer, m->tee,

					 m->eq_queue, m->volume, m->audio_sink,
					 m->level_queue, m->level, m->level_sink,
					 m->spectrum_queue, m->spectrum, m->spectrum_sink,

					 nullptr);

	/* before tee */
	success = gst_element_link_many(m->audio_convert, m->equalizer, m->tee,  nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link audio convert with tee")){
		return false;
	}

	/* standard output branch */
	success = gst_element_link_many(m->eq_queue, m->volume, /*_speed,*/ m->audio_sink, nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link eq with audio sink")) {
		return false;
	}


	/* level branch */
	success = gst_element_link_many(m->level_queue, m->level, m->level_sink, nullptr);
	if(!_test_and_error_bool(success, "Engine: Cannot link Level pipeline")){
		return false;
    }


	/* spectrum branch */
	success = gst_element_link_many(m->spectrum_queue, m->spectrum, m->spectrum_sink, nullptr);

	if(!_test_and_error_bool(success, "Engine: Cannot link Spectrum pipeline")){
		return false;
    }


	/* lame branch (optional) */
	if(m->lame){
		gst_bin_add_many(GST_BIN(_pipeline), m->lame_queue,  m->lame_converter, m->lame_resampler, m->lame, m->lame_app_sink, nullptr);
		success = gst_element_link_many( m->lame_queue, m->lame_converter, m->lame_resampler, m->lame, m->lame_app_sink, nullptr);
		_test_and_error_bool(success, "Engine: Cannot link lame stuff");
    }

	/* stream rippper branch (optional) */
	if(m->file_sink){
		gst_bin_add_many(GST_BIN(_pipeline), m->file_queue, m->file_converter, m->file_resampler, m->file_lame, m->file_sink, nullptr);
		success = gst_element_link_many( m->file_queue, m->file_converter, m->file_resampler, m->file_lame, m->file_sink, nullptr);
		_test_and_error_bool(success, "Engine: Cannot link streamripper stuff");
    }

	/* create tee pads */
	tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (m->tee), "src_%u");
	if(!_test_and_error(tee_src_pad_template, "Engine: _tee_src_pad_template is nullptr")) {
		return false;
	}

	/* connect branches with tee */
	success = tee_connect(m->tee, tee_src_pad_template, m->level_queue, "Level");
	if(!_test_and_error_bool(success, "Engine: Cannot link level queue with tee")){
		return false;
    }

	tee_connect(m->tee, tee_src_pad_template, m->spectrum_queue, "Spectrum");
	if(!_test_and_error_bool(success, "Engine: Cannot link spectrum queue with tee")){
		return false;
    }

	tee_connect(m->tee, tee_src_pad_template, m->eq_queue, "Equalizer");
	if(!_test_and_error_bool(success, "Engine: Cannot link eq queue with tee")){
		return false;
	}

	if(m->lame){
		success = tee_connect(m->tee, tee_src_pad_template, m->lame_queue, "Lame");
		if(!_test_and_error_bool(success, "Engine: Cannot link lame queue with tee")){
			_settings->set(SetNoDB::MP3enc_found, false);
		}
	}

	if(m->file_sink){
		success = tee_connect(m->tee, tee_src_pad_template, m->file_queue, "Streamripper");
		if(!_test_and_error_bool(success, "Engine: Cannot link streamripper stuff")){
			_settings->set(Set::Engine_SR_Active, false);
		}
    }

	return true;
}

bool PlaybackPipeline::configure_elements()
{
	guint64 interval = 25000000;
	gint threshold = -75;
	QList<GstElement*> sinks;

	g_object_set (G_OBJECT (m->audio_src),
				  "use-buffering", true,
				  nullptr);

	g_object_set (G_OBJECT (m->level),
				  "post-messages", true,
				  "interval", interval,
				  nullptr);

	int bins = _settings->get(Set::Engine_SpectrumBins);
	g_object_set (G_OBJECT (m->spectrum),
				  "post-messages", true,
				  "interval", interval,
				  "bands", bins,
				  "threshold", threshold,
				  "message-phase", false,
				  "message-magnitude", true,
				  "multi-channel", false,
                  nullptr);

	m->init_equalizer();

	/* run synced and not as fast as we can */
	g_object_set(G_OBJECT (m->audio_sink),
				 "sync",
				 true,
				 nullptr);

	if(m->lame){
		g_object_set(G_OBJECT (m->lame),
					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 nullptr);

		gst_object_ref(m->lame_app_sink);
		g_object_set(G_OBJECT(m->lame_app_sink),
					 "emit-signals",
					 true, nullptr );

		sinks << m->lame_app_sink;
	}

	if(m->file_sink){
		g_object_set(G_OBJECT (m->file_lame),

					 "perfect-timestamp", true,
					 "target", 1,
					 "cbr", true,
					 "bitrate", 128,
					 "encoding-engine-quality", 2,
					 nullptr);

		g_object_set(G_OBJECT(m->file_sink),
					 "buffer-size", 8192,
					 "location", (Util::sayonara_path() + "bla.mp3").toLocal8Bit().data(),
					 nullptr);

		sinks << m->file_sink;

		gst_element_set_state(m->file_sink, GST_STATE_NULL);
	}

	sinks << m->level_sink << m->spectrum_sink;

	for(GstElement* sink : sinks){
		//gst_object_ref(sink);
		/* run synced and not as fast as we can */
		g_object_set(G_OBJECT (sink), "sync", true, nullptr);

		/* change the state, when it's said to change the state */
		g_object_set(G_OBJECT (sink), "async", false, nullptr);
	}

	g_signal_connect (m->audio_src, "pad-added", G_CALLBACK (PipelineCallbacks::decodebin_ready), m->audio_convert);
	g_signal_connect (m->audio_src, "source-setup", G_CALLBACK (PipelineCallbacks::source_ready), nullptr);
	if(m->lame){
		g_signal_connect (m->lame_app_sink, "new-sample", G_CALLBACK(PipelineCallbacks::new_buffer), this);
    }

	return true;
}

uint64_t PlaybackPipeline::get_about_to_finish_time() const
{
	return std::max<uint64_t>(this->get_fading_time_ms(),
							 AbstractPipeline::get_about_to_finish_time());
}

void PlaybackPipeline::play()
{
	AbstractPipeline::play();
}

void PlaybackPipeline::stop()
{
	AbstractPipeline::stop();

	abort_fader();
}

void PlaybackPipeline::s_vol_changed()
{
	m->vol = _settings->get(Set::Engine_Vol);

	float vol_val = (float) ((m->vol * 1.0f) / 100.0f);

	g_object_set(G_OBJECT(m->volume), "volume", vol_val, nullptr);
}


void PlaybackPipeline::s_mute_changed()
{
	bool muted = _settings->get(Set::Engine_Mute);
	g_object_set(G_OBJECT(m->volume), "mute", muted, nullptr);
}

void PlaybackPipeline::s_show_level_changed()
{
	m->show_level = _settings->get(Set::Engine_ShowLevel);
	Probing::handle_probe(&m->show_level, m->level_queue, &m->level_probe, Probing::level_probed);
}


void PlaybackPipeline::s_show_spectrum_changed()
{
	m->show_spectrum = _settings->get(Set::Engine_ShowSpectrum);

	Probing::handle_probe(&m->show_spectrum, m->spectrum_queue, &m->spectrum_probe, Probing::spectrum_probed);
}


void PlaybackPipeline::set_n_sound_receiver(int num_sound_receiver)
{
	if(!m->lame){
		return;
	}

	m->run_broadcast = (num_sound_receiver > 0);

	Probing::handle_probe(&m->run_broadcast, m->lame_queue, &m->lame_probe, Probing::lame_probed);
}

GstElement* PlaybackPipeline::get_source() const
{
	return m->audio_src;
}

GstElement* PlaybackPipeline::get_pipeline() const
{
	return _pipeline;
}

void PlaybackPipeline::force_about_to_finish()
{
	_about_to_finish = true;
	emit sig_about_to_finish(get_about_to_finish_time());
}


bool PlaybackPipeline::set_uri(gchar* uri)
{
	stop();

	g_object_set(G_OBJECT(m->audio_src), "uri", uri, nullptr);

	gst_element_set_state(_pipeline, GST_STATE_PAUSED);

	return true;
}


void PlaybackPipeline::set_eq_band(int band, int val)
{
	m->set_band(band, val);
}

void PlaybackPipeline::set_streamrecorder_path(const QString& path)
{
	m->set_streamrecorder_target_path(path);
}

gint64 PlaybackPipeline::seek_rel(double percent, gint64 ref_ns)
{
	return m->seek_rel(percent, ref_ns);
}

gint64 PlaybackPipeline::seek_abs(gint64 ns)
{
	return m->seek_abs(ns);
}

void PlaybackPipeline::set_current_volume(double volume)
{
	g_object_set(m->volume, "volume", volume, nullptr);
}


double PlaybackPipeline::get_current_volume() const
{
	double volume;
	g_object_get(m->volume, "volume", &volume, nullptr);
	return volume;
}


void PlaybackPipeline::s_speed_active_changed()
{
	if(!m->pitch){
		return;
	}

	gint64 pos;

	GstElement* source = get_source();
	bool active = _settings->get(Set::Engine_SpeedActive);

	gst_element_query_position(source, GST_FORMAT_TIME, &pos);

	if(active){
		add_element(m->pitch, m->audio_convert, m->equalizer);
        s_speed_changed();
	}

	else{
		remove_element(m->pitch, m->audio_convert, m->equalizer);
	}

	if(this->get_state() == GST_STATE_PLAYING)
	{
		m->seek_nearest(pos);
	}
}


void PlaybackPipeline::s_speed_changed()
{
	m->set_speed(
		_settings->get(Set::Engine_Speed),
		_settings->get(Set::Engine_Pitch) / 440.0,
		_settings->get(Set::Engine_PreservePitch)
	);
}
