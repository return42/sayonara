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
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/MetaDataList.h"

#include "Components/PlayManager/PlayManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>


struct StreamRecorder::Private
{
	QString			sr_recording_dst;				// recording destination
	QString			session_path;					// where to store the mp3 files of the session
	QString			session_playlist_name;			// playlist name
	MetaDataList	session_collector;				// gather all tracks of a session
	MetaData		md;							// current track

	int				cur_idx;							// index of track (used for filename)
	bool            recording;						// is in a session currently

};

static QString get_time_str()
{
    QString time_str;
    QDateTime cur = QDateTime::currentDateTime();

    QString weekday = cur.date().longDayName(cur.date().dayOfWeek() );
    QString year = QString::number(cur.date().year());
    QString month = cur.date().shortMonthName(cur.date().month());
    QString day = QString("%1").arg(cur.date().day(), 2, 10, QLatin1Char('0'));
    QString hr = QString("%1").arg(cur.time().hour(), 2, 10, QLatin1Char('0'));
    QString min = QString("%1").arg(cur.time().minute(), 2, 10, QLatin1Char('0'));

	time_str = weekday + "_" + year + "-" + month + "-" + day + "_" + hr + "h" + min;
    return time_str;
}


StreamRecorder::StreamRecorder(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	m = Pimpl::make<StreamRecorder::Private>();

	clear();

	QDir d(Helper::sayonara_path());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::soundfile_extensions());

	for( const QString& str : lst)
	{
		QString path = d.absoluteFilePath(str);
		QFile f(path);
		f.remove();

		sp_log(Log::Info) << "Remove " << path;
    }

	PlayManager* play_manager = PlayManager::getInstance();
	connect(play_manager, &PlayManager::sig_playstate_changed, this, &StreamRecorder::playstate_changed);
}

StreamRecorder::~StreamRecorder() {}

void StreamRecorder::clear()
{
	m->md.title = "";
	m->session_path = get_time_str();
	m->session_collector.clear();
	m->sr_recording_dst = "";
	m->cur_idx = 1;
}

void StreamRecorder::new_session()
{
	clear();
	sp_log(Log::Info) << "New session: " << m->session_path;
}


QString StreamRecorder::change_track(const MetaData& md)
{
	QString sr_path;
	QString session_path;
	QString title;

	if(!m->recording){
		return "";
	}

	if(md.title == m->md.title) {
		return m->sr_recording_dst;
	}

	bool saved = save();
	if(saved){
		m->cur_idx++;
	}

	if(!Helper::File::is_www(md.filepath())) {
		m->recording = false;
		m->sr_recording_dst = "";
		return "";
	}
	
	m->md = md;
	m->md.year = QDateTime::currentDateTime().date().year();
	m->md.track_num = m->cur_idx;
	
	title = QString("%1 - %2 - %3")
			.arg(m->cur_idx, 3, 10, QLatin1Char('0'))
			.arg(md.artist())
			.arg(md.title);

	title.replace("/", "_");
	title.replace("\\", "_");
	title.replace(":", "");

	sr_path = _settings->get(Set::Engine_SR_Path);
	session_path = check_session_path(sr_path);

	if(session_path.isEmpty()){
		m->sr_recording_dst = "";
		m->session_playlist_name = "";
		m->recording = false;
	}

	else{
		m->session_playlist_name = session_path + "/playlist.m3u";
		m->sr_recording_dst = session_path + "/" + title + ".mp3";
	}

	return m->sr_recording_dst;
}


bool  StreamRecorder::save()
{
	if(!QFile::exists(m->sr_recording_dst)){
        return false;
    }

	QFileInfo file_info(m->sr_recording_dst);
	if(file_info.size() < 20000){
		return false;
	}

	sp_log(Log::Info) << "Finalize file " << m->sr_recording_dst;

	m->md.set_filepath(m->sr_recording_dst);

	Tagging::setMetaDataOfFile(m->md);
	m->session_collector.push_back(m->md);

	PlaylistParser::save_playlist(m->session_playlist_name, m->session_collector, true);
	
    return true;
}


QString StreamRecorder::check_session_path(const QString& sr_path)
{
	bool create_session_path =_settings->get(Set::Engine_SR_SessionPath);

	if(!create_session_path) {
		return sr_path;
	}

	QString recording_dst = Helper::File::clean_filename(sr_path + "/" + m->session_path);
    if(!QFile::exists(recording_dst)) {
		Helper::File::create_directories(recording_dst);
    }

	QFileInfo fi(recording_dst);
	
	if(!fi.isWritable()){
		return "";
	}

    return recording_dst;
}

void StreamRecorder::record(bool b)
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


bool StreamRecorder::is_recording() const
{
	return m->recording;
}


void StreamRecorder::playstate_changed(PlayState state){
	if(state == PlayState::Stopped){
		if(m->recording){
			save();
			clear();
		}
	}
}
