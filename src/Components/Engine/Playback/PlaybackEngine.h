/* PlaybackEngine.h */

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


#ifndef GSTPLAYBACKENGINE_H_
#define GSTPLAYBACKENGINE_H_



#include "SoundOutReceiver.h"

#include "Components/Engine/AbstractEngine.h"

#include "Helper/EqualizerPresets.h"

#include <glib.h>
#include <gst/gst.h>
#include <gst/gstbuffer.h>

#include <QTimer>

class PlaybackPipeline;
class StreamRecorder;


enum class GaplessState : quint8 {
	NoGapless=0,		// no gapless enabled at all
	AboutToFinish,		// the phase when the new track is already displayed but not played yet
	TrackFetched,		// track is requested, but no yet there
	Playing				// currently playing
};

class PlaybackEngine : public Engine {

	Q_OBJECT


signals:

	void sig_data(const uchar* data, quint64 n_bytes);


public:

	PlaybackEngine(QObject* parent=nullptr);
	virtual ~PlaybackEngine();

	bool init() override;

	void set_track_finished() override;
	void update_duration() override;
	void update_bitrate(quint32 br) override;
	void set_about_to_finish(qint64 time2go) override;
	void set_cur_position_ms(qint64 pos_ms) override;

	void set_streamrecorder_recording(bool b);

	void set_spectrum(const QVector<float>& vals);
	void set_spectrum_receiver(SpectrumReceiver* receiver);

	void set_level(float left, float right);
	void set_level_receiver(LevelReceiver* receiver);

	void set_n_sound_receiver(int num_sound_receiver);

	void change_equalizer(int band, int value);
	void set_speed(float f);

	void emit_buffer(float inv_array_elements, float scale);



public slots:

	void play() override;
	void stop() override;
	void pause() override;

	void jump_abs_ms(quint64 pos_ms) override;
	void jump_rel_ms(quint64 pos_ms) override;
	void jump_rel(double percent) override;
	void update_md(const MetaData&) override;
	void update_cover(const QImage& img) override;
	void change_track(const MetaData&) override;
	void change_track(const QString&) override;

	void set_track_ready() override;
	void buffering(int progress) override;

	void gapless_timed_out();



private:
	
	PlaybackPipeline*		_pipeline=nullptr;
	PlaybackPipeline*		_other_pipeline=nullptr;

	LevelReceiver*			_level_receiver=nullptr;
	SpectrumReceiver*		_spectrum_receiver=nullptr;
	QTimer*				_timer=nullptr;

	GaplessState			_gapless_state;

	bool				_sr_active;

	StreamRecorder*			_stream_recorder=nullptr;

private:

	bool set_uri(const QString& filepath);
	void change_track_gapless(const MetaData& md);
	void change_gapless_state(GaplessState state);


private slots:
	void _gapless_changed();
	void _streamrecorder_active_changed();

};


#endif /* GSTENGINE_H_ */
