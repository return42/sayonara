/* ConvertEngine.cpp */

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

#include "ConvertEngine.h"
#include "ConvertPipeline.h"

#include "Components/Engine/Callbacks/PipelineCallbacks.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QUrl>

struct ConvertEngine::Private
{
    ConvertPipeline*		pipeline=nullptr;
    MetaData				md_target;
    gchar*					target_uri=nullptr;

    Private(Engine* parent)
    {
        pipeline = new ConvertPipeline(parent);
    }
};

ConvertEngine::ConvertEngine(QObject *parent) :
    Engine(EngineName::ConvertEngine, parent)
{
    m = Pimpl::make<Private>(this);

    connect(m->pipeline, &ConvertPipeline::sig_pos_changed_ms, this, &ConvertEngine::cur_pos_ms_changed);
}

ConvertEngine::~ConvertEngine() {}

bool ConvertEngine::init()
{
	return m->pipeline->init();
}


void ConvertEngine::change_track(const MetaData& md)
{
	stop();

	set_metadata(md);
    configure_target(md);
}

void ConvertEngine::change_track(const QString& str)
{
	Q_UNUSED(str);
}


void ConvertEngine::play()
{
	m->pipeline->play();
}

void ConvertEngine::pause()
{
	return;
}

void ConvertEngine::stop()
{
	m->pipeline->stop();

    Tagging::setMetaDataOfFile(m->md_target);
}

// public from Gstreamer Callbacks
void ConvertEngine::set_track_finished(GstElement* src)
{
	Q_UNUSED(src)
	emit sig_track_finished();
}

void ConvertEngine::cur_pos_ms_changed(int64_t v)
{
	sp_log(Log::Debug, this) << "Position changed " << v;

	emit sig_pos_changed_s((uint32_t) (v / 1000));
}

void ConvertEngine::set_volume(int vol) {Q_UNUSED(vol);}

bool ConvertEngine::set_uri(char* uri)
{
	return m->pipeline->set_uri(uri);
}

void ConvertEngine::configure_target(const MetaData& md)
{
    if(m->target_uri){
        g_free(m->target_uri);
        m->target_uri = nullptr;
    }

    QString filename = Util::File::get_filename_of_path(md.filepath());
    int idx = filename.lastIndexOf(".");
    if(idx > 0) {
        filename = filename.left(idx);
    }

    QString cvt_target_path = _settings->get(Set::Engine_CovertTargetPath);
    filename = cvt_target_path + "/" + filename + ".mp3";

    gchar* target_uri = filename.toUtf8().data();

    m->target_uri = g_strdup(target_uri);
    m->pipeline->set_target_uri(m->target_uri);

    m->md_target = md;
    m->md_target.set_filepath(filename);
}

void ConvertEngine::jump_abs_ms(uint64_t pos_ms) { Q_UNUSED(pos_ms); }

void ConvertEngine::jump_rel_ms(uint64_t ms) { Q_UNUSED(ms); }

void ConvertEngine::jump_rel(double percent) { Q_UNUSED(percent); }
