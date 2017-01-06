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

	bool            recording;						// is in a session currently
	int				cur_idx;							// index of track (used for filename)
};

static QString get_time_str() {

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
	_m = Pimpl::make<StreamRecorder::Private>();

	clear();

	QDir d(Helper::get_sayonara_path());

    // delete old stream ripper files
    QStringList lst = d.entryList(Helper::get_soundfile_extensions());

	for( const QString& str : lst) {
		sp_log(Log::Info) << "Remove " << d.absolutePath() + QDir::separator() + str;
        QFile f(d.absolutePath() + QDir::separator() + str);
        f.remove();
    }

	PlayManager* play_manager = PlayManager::getInstance();
	connect(play_manager, &PlayManager::sig_playstate_changed, this, &StreamRecorder::playstate_changed);
}

StreamRecorder::~StreamRecorder() {}

void StreamRecorder::clear(){
	_m->md.title = "";
	_m->session_path = get_time_str();
	_m->session_collector.clear();
	_m->sr_recording_dst = "";
	_m->cur_idx = 1;
}

void StreamRecorder::new_session(){

	clear();
	sp_log(Log::Info) << "New session: " << _m->session_path;
}


QString StreamRecorder::change_track(const MetaData& md) {

	QString sr_path;
	QString session_path;
	QString title;

	if(!_m->recording){
		return "";
	}

	if(md.title == _m->md.title) {
		return _m->sr_recording_dst;
	}

	save();

	if(!Helper::File::is_www(md.filepath())) {
		_m->recording = false;
		_m->sr_recording_dst = "";
		return "";
	}
	
	_m->md = md;
	_m->md.year = QDateTime::currentDateTime().date().year();
	_m->md.track_num = _m->cur_idx;
	
	title = QString("%1").arg(_m->cur_idx, 3, 10, QLatin1Char('0')) + "_" + md.title;
	title.replace(" ", "_");
	title.replace("/", "_");
	title.replace("\\", "_");
	title.replace(":", "");

	sr_path = _settings->get(Set::Engine_SR_Path);
	session_path = check_session_path(sr_path);

	if(session_path.isEmpty()){
		_m->sr_recording_dst = "";
		_m->session_playlist_name = "";
		_m->recording = false;
	}

	else{
		_m->session_playlist_name = session_path + "/playlist.m3u";
		_m->sr_recording_dst = session_path + "/" + title + ".mp3";
	}

	_m->cur_idx++;

	return _m->sr_recording_dst;
}


bool  StreamRecorder::save() {

	if(!QFile::exists(_m->sr_recording_dst)){
        return false;
    }

	sp_log(Log::Info) << "Finalize file " << _m->sr_recording_dst;

	_m->md.set_filepath(_m->sr_recording_dst);

	Tagging::setMetaDataOfFile(_m->md);
	_m->session_collector.push_back(_m->md);

	PlaylistParser::save_playlist(_m->session_playlist_name, _m->session_collector, true);
	
    return true;
}


QString StreamRecorder::check_session_path(const QString& sr_path) {

	bool create_session_path =_settings->get(Set::Engine_SR_SessionPath);

	if(!create_session_path) {
		return sr_path;
	}

	QString recording_dst = sr_path + QDir::separator() + _m->session_path;
    if(!QFile::exists(recording_dst)) {

		Helper::File::create_directories(recording_dst);
    }

	QFileInfo fi(recording_dst);
	
	if(!fi.isWritable()){
		return "";
	}

    return recording_dst;
}

void StreamRecorder::record(bool b){

	if(b == _m->recording) {
		return;
	}

	sp_log(Log::Debug) << "Stream recorder: activate: " << b;

    if(b){
        new_session();
    }

    else{
        save();
		clear();
    }

	_m->recording = b;
}


bool StreamRecorder::is_recording() const
{
	return _m->recording;
}


void StreamRecorder::playstate_changed(PlayState state){

	if(state == PlayState::Stopped){

		if(_m->recording){
			save();
			clear();
		}
	}
}
