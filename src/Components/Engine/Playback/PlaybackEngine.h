/* PlaybackEngine.h */

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

#ifndef GSTPLAYBACKENGINE_H_
#define GSTPLAYBACKENGINE_H_

#include "Components/Engine/AbstractEngine.h"

#include <QList>

class QTimer;
class QString;
class PlaybackPipeline;
class StreamRecorder;
class SpectrumReceiver;
class LevelReceiver;

/**
 * @brief The GaplessState enum
 * @ingroup Engine
 */
enum class GaplessState : unsigned char
{
	NoGapless=0,		// no gapless enabled at all
	AboutToFinish,		// the phase when the new track is already displayed but not played yet
	TrackFetched,		// track is requested, but no yet there
	Playing				// currently playing
};

/**
 * @brief The PlaybackEngine class
 * @ingroup Engine
 */
class PlaybackEngine :
		public Engine
{
	Q_OBJECT
    PIMPL(PlaybackEngine)

signals:
    void sig_data(const unsigned char* data, uint64_t n_bytes);

public:
	explicit PlaybackEngine(QObject* parent=nullptr);
	~PlaybackEngine();

	bool init() override;
    void init_other_pipeline();

	void set_track_finished(GstElement* src) override;

	void update_bitrate(uint32_t br, GstElement* src) override;
	void update_duration(GstElement* src) override;
	void set_about_to_finish(int64_t time2go) override;
	void set_cur_position_ms(int64_t pos_ms) override;

	void set_streamrecorder_recording(bool b);

	int get_spectrum_bins() const;
	void set_spectrum(const QList<float>& vals);
	void add_spectrum_receiver(SpectrumReceiver* receiver);

	void set_level(float left, float right);
	void add_level_receiver(LevelReceiver* receiver);

	void set_n_sound_receiver(int num_sound_receiver);

	void set_equalizer(int band, int value);

	void emit_buffer(float inv_array_elements, float scale);

public slots:

	void play() override;
	void stop() override;
	void pause() override;

	void jump_abs_ms(uint64_t pos_ms) override;
	void jump_rel_ms(uint64_t pos_ms) override;
	void jump_rel(double percent) override;
	void update_md(const MetaData& md, GstElement* src) override;
	void update_cover(const QImage& img, GstElement* src) override;
	void change_track(const MetaData& md) override;
	void change_track(const QString& filepath) override;

	void set_track_ready(GstElement* src) override;
	void set_buffer_state(int progress, GstElement* src) override;

	void gapless_timed_out();

private:
    bool set_uri(const QString& filepath) override;
	void change_track_gapless(const MetaData& md);
	void change_gapless_state(GaplessState state);


private slots:
	void _playlist_mode_changed();
	void _streamrecorder_active_changed();
};

#endif /* GSTENGINE_H_ */
