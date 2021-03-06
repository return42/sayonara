/* PlaybackEngine.cpp */

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

#include "PlaybackEngine.h"
#include "PlaybackPipeline.h"
#include "StreamRecorder.h"
#include "Components/Engine/Callbacks/EngineCallbacks.h"

#include "Utils/MetaData/MetaData.h"
#include "Utils/FileUtils.h"
#include "Utils/Playlist/PlaylistMode.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <algorithm>

using Engine::Playback;

/**
 * @brief The GaplessState enum
 * @ingroup Engine
 */
enum class GaplessState : uint8_t
{
	NoGapless=0,		// no gapless enabled at all
	AboutToFinish,		// the phase when the new track is already displayed but not played yet
	TrackFetched,		// track is requested, but no yet there
	Playing,			// currently playing
	Stopped
};

struct Playback::Private
{
	Pipeline::Playback*         pipeline=nullptr;
	Pipeline::Playback*         other_pipeline=nullptr;

	QList<LevelReceiver*>		level_receiver;
	QList<SpectrumReceiver*>	spectrum_receiver;

	StreamRecorder::StreamRecorder*   stream_recorder=nullptr;
	QTimer*						gapless_timer=nullptr;

	GaplessState                gapless_state;

	bool						sr_active;

	Private(QObject* parent) :
		gapless_state(GaplessState::Stopped),
		sr_active(false)
	{
		stream_recorder = new StreamRecorder::StreamRecorder(parent);

		gapless_timer = new QTimer();
		gapless_timer->setTimerType(Qt::PreciseTimer);
		gapless_timer->setSingleShot(true);
	}

	~Private()
	{
		while(gapless_timer && gapless_timer->isActive())
		{
			gapless_timer->stop();
		}

		if(gapless_timer){
			delete gapless_timer; gapless_timer = nullptr;
		}

		delete pipeline; pipeline = nullptr;
		if(other_pipeline)
		{
			delete other_pipeline; other_pipeline = nullptr;
		}

		delete stream_recorder; stream_recorder = nullptr;
	}

	void change_gapless_state(GaplessState state)
	{
		Settings* settings = Settings::instance();
		Playlist::Mode plm = settings->get(Set::PL_Mode);

		bool gapless = Playlist::Mode::isActiveAndEnabled(plm.gapless());
		bool crossfader = settings->get(Set::Engine_CrossFaderActive);

		gapless_state = state;

		if(!gapless && !crossfader) {
			gapless_state = GaplessState::NoGapless;
		}
	}
};

Playback::Playback(QObject* parent) :
	Base(Name::PlaybackEngine, parent)
{
	m = Pimpl::make<Private>(this);
}


Playback::~Playback()
{
	if(m->stream_recorder->is_recording())
	{
		set_streamrecorder_recording(false);
	}
}


bool Playback::init()
{
	gst_init(0, 0);

	bool success = init_pipeline(&m->pipeline);
	if(!success){
		return false;
	}

	Set::listen(Set::Engine_SR_Active, this, &Playback::s_streamrecorder_active_changed);
	Set::listen(Set::PL_Mode, this, &Playback::s_gapless_changed);
	Set::listen(Set::Engine_CrossFaderActive, this, &Playback::s_gapless_changed);

	connect(m->gapless_timer, &QTimer::timeout, [=](){
		m->pipeline->play();
	});

	return true;
}

bool Playback::init_pipeline(Pipeline::Playback** pipeline)
{
	if(*pipeline){
		return true;
	}

	*pipeline = new Pipeline::Playback(this);
	Pipeline::Playback* p = *pipeline;

	if(!p->init()){
		m->change_gapless_state(GaplessState::NoGapless);
		return false;
	}

	connect(p, &Pipeline::Playback::sig_about_to_finish, this, &Playback::set_track_almost_finished);
	connect(p, &Pipeline::Playback::sig_pos_changed_ms, this, &Playback::cur_pos_ms_changed);
	connect(p, &Pipeline::Playback::sig_data, this, &Playback::sig_data);

	return true;
}

bool Playback::change_track_crossfading(const MetaData& md)
{
	std::swap(m->pipeline, m->other_pipeline);

	m->other_pipeline->fade_out();

	if (!Base::change_track(md)) {
		return false;
	}

	m->pipeline->fade_in();
	m->change_gapless_state(GaplessState::Playing);

	return true;
}

bool Playback::change_track_gapless(const MetaData& md)
{
	std::swap(m->pipeline, m->other_pipeline);

	if (!Base::change_track(md)) {
		return false;
	}

	int64_t time_to_go = m->other_pipeline->get_time_to_go();
	if(time_to_go <= 0) {
		m->pipeline->play();
	}

	else {
		m->gapless_timer->setInterval(time_to_go);
		m->gapless_timer->start();

		sp_log(Log::Develop, this) << "Will start playing in " << time_to_go << "msec";
	}

	m->change_gapless_state(GaplessState::TrackFetched);

	return true;
}

bool Playback::change_track_immediatly(const MetaData& md)
{
	if(m->other_pipeline) {
		m->other_pipeline->stop();
	}

	return Base::change_track(md);
}

bool Playback::change_track(const MetaData& md)
{
	if(m->gapless_timer){
		m->gapless_timer->stop();
	}

	bool crossfader_active = _settings->get(Set::Engine_CrossFaderActive);
	if(m->gapless_state != GaplessState::Stopped && crossfader_active)
	{
		return change_track_crossfading(md);
	}

	else if(m->gapless_state == GaplessState::AboutToFinish )
	{
		return change_track_gapless(md);
	}

	return change_track_immediatly(md);
}

bool Playback::change_metadata(const MetaData& md)
{
	bool success = Base::change_metadata(md);
	if(!success)
	{
		m->change_gapless_state(GaplessState::Stopped);
	}

	return success;
}

bool Playback::change_uri(char* uri)
{
	return m->pipeline->set_uri(uri);
}

void Playback::play()
{
	if( m->gapless_state == GaplessState::AboutToFinish ||
		m->gapless_state == GaplessState::TrackFetched)
	{
		return;
	}

	m->pipeline->play();

	if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}

	m->change_gapless_state(GaplessState::Playing);
}


void Playback::stop()
{
	m->change_gapless_state(GaplessState::Stopped);

	if(m->gapless_timer){
		m->gapless_timer->stop();
	}

	sp_log(Log::Info, this) << "Stop";
	m->pipeline->stop();

	if(m->other_pipeline){
		m->other_pipeline->stop();
	}

	if(m->gapless_timer){
		m->gapless_timer->stop();
	}

	if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(false);
	}

	Base::stop();
}


void Playback::pause()
{
	m->pipeline->pause();
}


void Playback::jump_abs_ms(uint64_t pos_ms)
{
	m->pipeline->seek_abs(pos_ms * GST_MSECOND);
}

void Playback::jump_rel_ms(uint64_t ms)
{
	uint64_t new_time_ms = m->pipeline->get_source_position_ms() + ms;
	m->pipeline->seek_abs(new_time_ms * GST_MSECOND);
}


void Playback::jump_rel(double percent)
{
	m->pipeline->seek_rel(percent, metadata().length_ms * GST_MSECOND);
}



void Playback::cur_pos_ms_changed(int64_t pos_ms)
{
	if(sender() != m->pipeline){
		return;
	}

	if(pos_ms < 0 && Util::File::is_www(metadata().filepath())){
		return;
	}

	Base::set_current_position_ms(pos_ms);
}


void Playback::set_track_ready(GstElement* src)
{
	if(m->pipeline->has_element(src)){
		Base::set_track_ready(src);
	}
}

void Playback::set_track_almost_finished(int64_t time2go)
{
	Q_UNUSED(time2go)

	if(sender() != m->pipeline){
		return;
	}

	if( m->gapless_state == GaplessState::NoGapless ||
		m->gapless_state == GaplessState::AboutToFinish )
	{
		Base::set_track_almost_finished(time2go);
		return;
	}

	sp_log(Log::Debug, this) << "About to finish: " <<
								(int) m->gapless_state << " (" << time2go << "ms)";

	m->change_gapless_state(GaplessState::AboutToFinish);

	bool crossfade = _settings->get(Set::Engine_CrossFaderActive);
	if(crossfade){
		m->pipeline->fade_out();
	}

	emit sig_track_finished();
}


void Playback::set_track_finished(GstElement* src)
{
	if(m->pipeline->has_element(src))
	{
		Base::set_track_finished(src);
	}

	if(m->other_pipeline && m->other_pipeline->has_element(src))
	{
		sp_log(Log::Debug, this) << "Old track finished";

		m->other_pipeline->stop();
		m->change_gapless_state(GaplessState::Playing);
	}
}


void Playback::set_equalizer(int band, int val)
{
	m->pipeline->set_eq_band(band, val);

	if(m->other_pipeline){
		m->other_pipeline->set_eq_band(band, val);
	}
}


void Playback::set_buffer_state(int progress, GstElement* src)
{
	if(!Util::File::is_www(metadata().filepath())){
		progress = -1;
	}

	if(!m->pipeline->has_element(src)){
		progress = -1;
	}

	Base::set_buffer_state(progress, src);
}


void Playback::s_gapless_changed()
{
	Playlist::Mode plm = _settings->get(Set::PL_Mode);
	bool gapless =	(Playlist::Mode::isActiveAndEnabled(plm.gapless()) ||
					 _settings->get(Set::Engine_CrossFaderActive));

	if(gapless)
	{
		bool success = init_pipeline(&m->other_pipeline);

		if(success){
			m->change_gapless_state(GaplessState::Stopped);
			return;
		}
	}

	m->change_gapless_state(GaplessState::NoGapless);
}


void Playback::s_streamrecorder_active_changed()
{
	m->sr_active = _settings->get(Set::Engine_SR_Active);

	if(!m->sr_active){
		set_streamrecorder_recording(false);
	}
}


void Playback::set_streamrecorder_recording(bool b)
{
	QString dst_file;

	if(m->stream_recorder->is_recording() != b){
		m->stream_recorder->record(b);
	}

	if(b)
	{
		dst_file = m->stream_recorder->change_track(metadata());
		if(dst_file.isEmpty()){
			return;
		}
	}

	if(m->pipeline) {
		m->pipeline->set_streamrecorder_path(dst_file);
	}
}

void Playback::set_n_sound_receiver(int num_sound_receiver)
{
	m->pipeline->set_n_sound_receiver(num_sound_receiver);

	if(m->other_pipeline)
	{
		m->other_pipeline->set_n_sound_receiver(num_sound_receiver);
	}
}

void Playback::update_cover(const QImage& img, GstElement* src)
{
	if( m->pipeline->has_element(src) )
	{
		Base::update_cover(img, src);
	}
}


void Playback::update_metadata(const MetaData& md, GstElement* src)
{
	if(!m->pipeline->has_element(src)){
		return;
	}

	if(!Util::File::is_www( metadata().filepath() )) {
		return;
	}

	if(md.title().isEmpty()) {
		return;
	}

	QString title = md.title();
	QStringList splitted = md.title().split("-");
	if(splitted.size() == 2) {
		title = splitted[1].trimmed();
	}

	if(metadata().title().compare(title) == 0)
	{
		return;
	}

	set_current_position_ms(0);

	MetaData md_update = metadata();
	if(splitted.size() == 2){
		md_update.set_artist(splitted[0].trimmed());
		md_update.set_title(splitted[1].trimmed());
	}

	else {
		md_update.set_title(md.title());
	}

	Base::update_metadata(md_update, src);

	if(m->sr_active && m->stream_recorder->is_recording()){
		set_streamrecorder_recording(true);
	}
}


void Playback::update_duration(int64_t duration_ms, GstElement* src)
{
	if(! m->pipeline->has_element(src)){
		return;
	}

	m->pipeline->update_duration_ms(duration_ms, src);

	Base::update_duration(duration_ms, src);

}


void Playback::update_bitrate(uint32_t br, GstElement* src)
{
	if(!m->pipeline->has_element(src)){
		return;
	}

	Base::update_bitrate(br, src);
}


void Playback::add_spectrum_receiver(SpectrumReceiver* receiver)
{
	m->spectrum_receiver << receiver;
}

int Playback::get_spectrum_bins() const
{
	return _settings->get(Set::Engine_SpectrumBins);
}

void Playback::set_spectrum(const SpectrumList& vals)
{
	for(SpectrumReceiver* rcv : m->spectrum_receiver)
	{
		if(rcv){
			rcv->set_spectrum(vals);
		}
	}
}


void Playback::add_level_receiver(LevelReceiver* receiver)
{
	m->level_receiver << receiver;
}

void Playback::set_level(float left, float right)
{
	for(LevelReceiver* rcv : m->level_receiver)
	{
		if(rcv){
			rcv->set_level(left, right);
		}
	}
}

