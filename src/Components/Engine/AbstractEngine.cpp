/* Engine.cpp */

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

#include "AbstractEngine.h"
#include "Helper/FileHelper.h"
#include "Helper/Logger/Logger.h"
#include <QUrl>

#include <gst/gst.h>

struct Engine::Private
{
    gchar*          uri=nullptr;
    EngineName      name;

    Private(EngineName name) :
        name(name)
    {}
};

Engine::Engine(EngineName name, QObject *parent) :
	QObject(parent),
    SayonaraClass(),
    _cur_pos_ms(0)
{
    m = Pimpl::make<Private>(name);
}

Engine::~Engine() {}

EngineName Engine::get_name() const
{
    return m->name;
}

void Engine::set_track_finished(GstElement* src)
{
	Q_UNUSED(src)
}

/*void Engine::async_done(GstElement* src){
	Q_UNUSED(src)

}*/

void Engine::update_md(const MetaData& md, GstElement* src)
{
	Q_UNUSED(src)
	Q_UNUSED(md)
}

void Engine::update_cover(const QImage& img, GstElement* src)
{
	Q_UNUSED(src)
	Q_UNUSED(img);
}

void Engine::update_duration(GstElement* src)
{
	Q_UNUSED(src)
}


void Engine::update_bitrate(uint32_t br, GstElement* src)
{
	Q_UNUSED(src)
	Q_UNUSED(br)
}

void Engine::set_track_ready(GstElement* src)
{
	Q_UNUSED(src)
}

void Engine::set_about_to_finish(int64_t ms)
{
	Q_UNUSED(ms)
}

void Engine::set_cur_position_ms(int64_t ms)
{
	_cur_pos_ms = ms;
	emit sig_pos_changed_ms(ms);
}

bool Engine::set_uri(const QString &filepath)
{
    QUrl url;

    if(m->uri)
    {
        g_free(m->uri);
        m->uri = nullptr;
    }

    bool playing_stream = Helper::File::is_www(filepath);

    // stream, but don't want to record
    // stream is already uri
    if (playing_stream)
    {
        url = QUrl(filepath);
        m->uri = g_strdup(url.toString().toUtf8().data());
    }

    // no stream (not quite right because of mms, rtsp or other streams
    // normal filepath -> no uri
    else if (!filepath.contains("://"))
    {
        url = QUrl::fromLocalFile(filepath);
        m->uri = g_strdup(url.url().toUtf8().data());
    }

    else {
        m->uri = g_strdup(filepath.toUtf8().data());
    }

    if(g_utf8_strlen(m->uri, 1024) == 0) {
        sp_log(Log::Warning) << "uri = 0";
        return false;
    }

    return true;
}

char* Engine::get_uri() const
{
    return m->uri;
}


void Engine::set_buffer_state(int progress, GstElement* src)
{
    Q_UNUSED(progress)
    Q_UNUSED(src)
}
