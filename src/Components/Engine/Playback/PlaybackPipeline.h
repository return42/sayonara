/* PlaybackPipeline.h */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#ifndef GSTPLAYBACKPIPELINE_H_
#define GSTPLAYBACKPIPELINE_H_

#include "Components/Engine/AbstractPipeline.h"

#include <gst/app/gstappsink.h>

struct StreamRecorderData;
class Engine;

class PlaybackPipeline : public AbstractPipeline
{
	Q_OBJECT


public:
	PlaybackPipeline(Engine* engine, QObject *parent=nullptr);
	virtual ~PlaybackPipeline();

	bool init(GstState state=GST_STATE_NULL) override;
	bool set_uri(gchar* uri) override;

	void set_n_sound_receiver(int num_sound_receiver);

	GstElement* get_source() const override;




public slots:

	void play() override;
	void pause() override;
	void stop() override;

	void set_eq_band(const QString& band_name, double val);

	void set_speed(float f);

	void set_streamrecorder_path(const QString& session_path);

	gint64 seek_rel(double percent, gint64 ref_ns);
	gint64 seek_abs(gint64 ns );


private:

	int					_vol;
	bool				_speed_active;
	float				_speed_val;
	QString				_sr_path;
	StreamRecorderData* _sr_data=nullptr;


	GstElement*			_audio_src=nullptr;
	GstElement*			_audio_convert=nullptr;
	GstElement*			_tee=nullptr;

	GstElement*			_eq_queue=nullptr;
	GstElement*			_equalizer=nullptr;
	GstElement*			_speed=nullptr;
	GstElement*			_volume=nullptr;

	GstElement*			_audio_sink=nullptr;

	GstElement*			_spectrum_queue=nullptr;
	GstElement*			_spectrum=nullptr;
	GstElement*			_spectrum_sink=nullptr;

	GstElement*			_level_queue=nullptr;
	GstElement*			_level=nullptr;
	GstElement*			_level_sink=nullptr;

	GstElement*			_lame_queue=nullptr;
	GstElement*			_lame_converter=nullptr;
	GstElement*			_lame_resampler=nullptr;
	GstElement*			_lame=nullptr;
	GstElement*			_lame_app_sink=nullptr;

	GstElement*			_file_queue=nullptr;
	GstElement*			_file_sink=nullptr;
	GstElement*			_file_resampler=nullptr;
	GstElement*			_file_lame=nullptr;

	GstState			_saved_state;

	gulong				_level_probe, _spectrum_probe, _lame_probe, _file_probe;
	bool				_show_level, _show_spectrum, _run_broadcast, _run_sr;

    bool _seek(gint64 ns);
	bool tee_connect(GstPadTemplate* tee_src_pad_template,
					 GstElement* queue,
					 const QString& queue_name
	);

	bool create_elements() override;
	bool add_and_link_elements() override;
	bool configure_elements() override;

	void init_equalizer();


protected slots:

	void _sl_vol_changed();
	void _sl_show_level_changed();
	void _sl_show_spectrum_changed();
	void _sl_mute_changed();

};

#endif
