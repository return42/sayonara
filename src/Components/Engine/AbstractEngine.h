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

#include <QObject>
#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Helper/Pimpl.h"


struct _GstElement;
typedef struct _GstElement GstElement;

class QImage;

/**
 * @brief The EngineName enum
 * @ingroup Engine
 */
enum class EngineName : int8_t
{
    Undefined=-1,
    EngineHandler=0,
	PlaybackEngine=1,
	ConvertEngine=2
};

/**
 * @brief The Engine class
 * @ingroup Engine
 */
class Engine :
		public QObject,
		protected SayonaraClass
{
	Q_OBJECT
    PIMPL(Engine)

public:
    explicit Engine(EngineName name, QObject* parent=nullptr);
	virtual ~Engine();
	virtual EngineName	get_name() const final;

	virtual bool		init()=0;

	virtual void		update_md(const MetaData& md, GstElement* src);
	virtual void		update_cover(const QImage& img, GstElement* src);
    virtual void		update_duration(int64_t duration_ms, GstElement* src);
	virtual void		update_bitrate(uint32_t br, GstElement* src);

	virtual void		set_track_ready(GstElement* src);
    virtual void        set_track_almost_finished(int64_t time2go);
    virtual void		set_track_finished(GstElement* src);

    virtual void		set_buffer_state(int percent, GstElement* src);

	void				set_level(float right, float left);
	void				set_spectrum(QList<float>& lst );

    virtual void        change_track(const MetaData& md);
    virtual void        change_track(const QString& filepath);


signals:
	void sig_md_changed(const MetaData& md);
	void sig_dur_changed(const MetaData& md);
	void sig_br_changed(const MetaData& md);

    void sig_pos_changed_ms(uint64_t pos_ms);
	void sig_buffering(int progress);

	void sig_track_ready();
    void sig_track_almost_finished(int64_t time2go);
	void sig_track_finished();

	void sig_error(const QString& message);

    void sig_download_progress(int);
	void sig_cover_changed(const QImage& img);


public slots:
	virtual void play()=0;
    virtual void stop();
	virtual void pause()=0;
	virtual void error(const QString& error);

	virtual void jump_abs_ms(uint64_t ms)=0;
	virtual void jump_rel_ms(uint64_t ms)=0;
	virtual void jump_rel(double ms)=0;


protected:
    virtual bool set_uri(char* uri)=0;

    virtual void        update_metadata(const MetaData& md);
    virtual bool		change_metadata(const MetaData& md);
    const MetaData&		metadata() const;

    void                set_current_position_ms(int64_t pos_ms);
    int64_t             current_position_ms() const;

};

extern Engine* gst_obj_ref;

#endif

