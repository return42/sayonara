/* StreamRecorder.cpp */

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

#include "StreamRecorder.h"

#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/Parser/PlaylistParser.h"
#include "Utils/Tagging/Tagging.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Settings/Settings.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/StreamRecorder/StreamRecorderUtils.h"

#include "Components/PlayManager/PlayManager.h"

#include <QDir>
#include <QFile>
#include <QDateTime>

using namespace StreamRecorder;

struct Accessor::Private
{
	QString			sr_recording_dst;				// recording destination
	QString			session_path;					// where to store the mp3 files of the session
	QString			session_playlist_name;			// playlist name
	MetaDataList	session_collector;				// gather all tracks of a session
    MetaData		md;                             // current track

    int				cur_idx;						// index of track (used for filename)
	bool            recording;						// is in a session currently
};


Accessor::Accessor(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	m = Pimpl::make<Accessor::Private>();

	clear();

	QDir d(Util::sayonara_path());

    // delete old stream ripper files
    QStringList lst = d.entryList(Util::soundfile_extensions());

	for( const QString& str : lst)
	{
		QString path = d.absoluteFilePath(str);
		QFile f(path);
		f.remove();

		sp_log(Log::Info) << "Remove " << path;
    }

	PlayManagerPtr play_manager = PlayManager::instance();
	connect(play_manager, &PlayManager::sig_playstate_changed, this, &Accessor::playstate_changed);
}

Accessor::~Accessor() {}

void Accessor::clear()
{
	m->md.title = "";
    m->session_path = "";
	m->session_collector.clear();
	m->sr_recording_dst = "";
    m->session_playlist_name.clear();
	m->cur_idx = 1;
}

void Accessor::new_session()
{
	clear();
	sp_log(Log::Info) << "New session: " << m->session_path;
}


QString Accessor::change_track(const MetaData& md)
{
    QString sr_path = _settings->get(Set::Engine_SR_Path);

	if(!m->recording){
		return "";
	}

	if(md.title == m->md.title) {
		return m->sr_recording_dst;
	}

	bool saved = save();
    if(saved) {
		m->cur_idx++;
    }

    if(!Util::File::is_www(md.filepath()))
    {
		m->sr_recording_dst = "";
        m->session_playlist_name = "";
        m->recording = false;
		return "";
	}

	m->md = md;
	m->md.year = QDateTime::currentDateTime().date().year();
	m->md.track_num = m->cur_idx;
	
    QString target_path_template = _settings->get(Set::Engine_SR_SessionPathTemplate);
    if(target_path_template.isEmpty())
    {
        bool use_session_path = _settings->get(Set::Engine_SR_SessionPath);
        target_path_template = Utils::target_path_template_default(use_session_path);
    }

    Utils::TargetPaths target_path = Utils::full_target_path(sr_path, target_path_template, m->md);
    if(target_path.first.isEmpty())
    {
		m->sr_recording_dst = "";
		m->session_playlist_name = "";
		m->recording = false;
        return "";
	}

    Util::File::create_directories(Util::File::get_parent_directory(target_path.first));

    m->sr_recording_dst = target_path.first;
    m->session_playlist_name = target_path.second;

	return m->sr_recording_dst;
}


bool  Accessor::save()
{
    if(!QFile::exists(m->sr_recording_dst)) {
        return false;
    }

	QFileInfo file_info(m->sr_recording_dst);
    if(file_info.size() < 20000) {
		return false;
	}

	sp_log(Log::Info) << "Finalize file " << m->sr_recording_dst;

	m->md.set_filepath(m->sr_recording_dst);

    Tagging::Util::setMetaDataOfFile(m->md);
	m->session_collector.push_back(m->md);

	PlaylistParser::save_playlist(m->session_playlist_name, m->session_collector, true);
	
    return true;
}


QString Accessor::check_target_path(const QString& target_path)
{
    if(!QFile::exists(target_path)) {
        Util::File::create_directories(Util::File::get_parent_directory(target_path));
    }

    QFileInfo fi(target_path);
	
	if(!fi.isWritable()){
		return "";
	}

    return target_path;
}

void Accessor::record(bool b)
{
	if(b == m->recording) {
		return;
	}

	sp_log(Log::Debug, this) << "Stream recorder: activate: " << b;

    if(b){
        new_session();
    }

    else{
        save();
		clear();
    }

	m->recording = b;
}


bool Accessor::is_recording() const
{
	return m->recording;
}


void Accessor::playstate_changed(PlayState state){
	if(state == PlayState::Stopped){
		if(m->recording){
			save();
			clear();
		}
	}
}
