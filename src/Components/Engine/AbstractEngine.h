/* Engine.h */

/* Copyright (C) 2012  Lucio Carreras
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


#ifndef ENGINE_H_
#define ENGINE_H_

#include "Helper/Helper.h"
#include "Helper/SayonaraClass.h"

#define PLAYBACK_ENGINE "playback_engine"
#define CONVERT_ENGINE "convert_engine"

enum class EngineName : quint8 {
	Undefined=0,
	PlaybackEngine=1,
	ConvertEngine=2
};


class Engine :
		public QObject,
		protected SayonaraClass
{

	Q_OBJECT


public:

	Engine(QObject* parent=nullptr);
	virtual EngineName	get_name() const final;

	virtual bool		init()=0;

	virtual void		set_track_finished();

	virtual void		async_done();
	virtual void		update_md(const MetaData&);
	virtual void		update_duration();
	virtual void		update_bitrate(quint32 br);
	virtual void		update_time(qint32 time);

	void				set_level(float right, float left);
	void				set_spectrum(QVector<float>& lst );

signals:
	void sig_md_changed(const MetaData&);
	void sig_dur_changed(const MetaData&);
	void sig_br_changed(const MetaData&);

	void sig_pos_changed_ms(quint64);
	void sig_pos_changed_s(quint32);

	void sig_buffering(int progress);

	void sig_track_ready();
	void sig_track_finished();

    void sig_download_progress(int);


protected slots:

	virtual void set_about_to_finish(qint64 ms);
	virtual void set_cur_position_ms(qint64 ms);



public slots:
	virtual void play()=0;
	virtual void stop()=0;
	virtual void pause()=0;

	virtual void jump_abs_ms(quint64 ms)=0;
	virtual void jump_rel_ms(quint64 ms)=0;
	virtual void jump_rel(double ms)=0;

	virtual void change_track(const MetaData&)=0;
	virtual void change_track(const QString&)=0;

	virtual void set_track_ready();
	virtual void buffering(int);


protected:

	char*		_uri=nullptr;

	EngineName	_name;

	MetaData	_md;
	qint64		_cur_pos_ms;
	bool 		_playing_stream;
	bool		_broadcast_active;

};

extern Engine* gst_obj_ref;

#endif

