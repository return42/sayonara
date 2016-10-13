/* LastFM.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Gfeneral Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * LastFM.cpp
 *
 *  Created on: Apr 19, 2011
 *      Author: Lucio Carreras
 */

#include "LastFM.h"
#include "LFMGlobals.h"
#include "LFMTrackChangedThread.h"
#include "LFMLoginThread.h"
#include "LFMWebAccess.h"

#include "Helper/Random/RandomGenerator.h"
#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Logger/Logger.h"

#include "Components/PlayManager/PlayManager.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/Playlist/AbstractPlaylist.h"
#include "Database/DatabaseConnector.h"

#include <QDomDocument>
#include <QUrl>

#include <algorithm>

struct LastFM::Private
{

	bool						logged_in;
	bool						active;
	bool						scrobbled;

	QString						username;
	QString						auth_token;
	QString						session_key;

	LFMTrackChangedThread*		track_changed_thread=nullptr;
	LFMLoginThread*				login_thread=nullptr;

	PlayManager*				play_manager=nullptr;

	quint64						old_pos;
	quint64						old_pos_difference;

	MetaData					md;
};

LastFM::LastFM() :
	QObject(),
	SayonaraClass()
{
	_m = Pimpl::make<LastFM::Private>();
	_m->play_manager = PlayManager::getInstance();

	_m->logged_in = false;
	_m->track_changed_thread = new LFMTrackChangedThread("", "", this);
	_m->login_thread = new LFMLoginThread(this);

	connect(_m->login_thread, &LFMLoginThread::sig_logged_in, this, &LastFM::sl_login_thread_finished);
	connect(_m->play_manager, &PlayManager::sig_track_changed,	this, &LastFM::sl_track_changed);
	connect(_m->play_manager, &PlayManager::sig_position_changed_ms, this, &LastFM::sl_position_ms_changed);
	connect(_m->track_changed_thread, &LFMTrackChangedThread::sig_similar_artists_available,
			this, &LastFM::sl_similar_artists_available);

	REGISTER_LISTENER_NO_CALL(Set::LFM_Login, psl_login);
	REGISTER_LISTENER(Set::LFM_Active, psl_login);
}

LastFM::~LastFM() {}

void LastFM::get_login(QString& user, QString& pw){

	StringPair user_pw = Settings::getInstance()->get(Set::LFM_Login);
	user = user_pw.first;
	pw = user_pw.second;
}


bool LastFM::is_logged_in() {
	return _m->logged_in;
}


void LastFM::psl_login() {

	_m->active = _settings->get(Set::LFM_Active);

	if(!_m->active){
		return;
	}

	QString password;
	get_login(_m->username, password);

	_m->logged_in = false;
	_m->login_thread->login(_m->username, password);
}


void LastFM::sl_login_thread_finished(bool success) {

	if(!success){
		return;
	}

	LFMLoginStuff login_info = _m->login_thread->getLoginStuff();

	_m->logged_in = login_info.logged_in;
	_m->auth_token = login_info.token;
	_m->session_key = login_info.session_key;

	_settings->set(Set::LFM_SessionKey, _m->session_key);

	sp_log(Log::Debug, "LastFM") << "Got session key";

	if(!_m->logged_in){
		sp_log(Log::Warning, "LastFM") << "Cannot login";
	}

	emit sig_logged_in(_m->logged_in);
}

void LastFM::sl_track_changed(const MetaData& md) {

	Playlist::Mode pl_mode = _settings->get(Set::PL_Mode);
	if( Playlist::Mode::isActiveAndEnabled(pl_mode.dynamic())) {
		_m->track_changed_thread->search_similar_artists(md);
	}

	if(!_m->active ) return;
	if(!_m->logged_in ) return;

	_m->md = md;

	_m->track_changed_thread->set_username(_m->username);
	_m->track_changed_thread->set_session_key(_m->session_key);

	reset_scrobble();

	_m->track_changed_thread->update_now_playing(md);
}


void LastFM::sl_position_ms_changed(quint64 pos_ms)
{
	if(!_m->active){
		return;
	}

	check_scrobble(pos_ms);
}


void LastFM::reset_scrobble()
{
	_m->scrobbled = false;
	_m->old_pos = 0;
	_m->old_pos_difference = 0;
}


bool LastFM::check_scrobble(quint64 pos_ms)
{
	if(!_m->logged_in){
		return false;
	}

	if(_m->scrobbled){
		return false;
	}

	if(_m->md.length_ms <= 0){
		return false;
	}

	if(_m->old_pos == 0){
		_m->old_pos = pos_ms;
		_m->old_pos_difference = 0;
		return false;
	}

	else{
		if(_m->old_pos > pos_ms){
			_m->old_pos = 0;
		}

		else if(pos_ms > _m->old_pos + 2000){
			_m->old_pos = 0;
		}

		else{

			quint64 scrobble_time_ms = (quint64) (_settings->get(Set::LFM_ScrobbleTimeSec) * 1000);

			_m->old_pos_difference += (pos_ms - _m->old_pos);
			_m->old_pos = pos_ms;

			if( (_m->old_pos_difference > scrobble_time_ms) ||
				(_m->old_pos_difference >= ((_m->md.length_ms  * 3) / 4) && _m->md.length_ms >= 1000))
			{
				scrobble(_m->md);
			}
		}
	}

	return _m->scrobbled;
}

void LastFM::scrobble(const MetaData& metadata) {

	_m->scrobbled = true;

	if(!_m->active) {
		return;
	}

	if(!_m->logged_in){
		return;
	}

	LFMWebAccess* lfm_wa = new LFMWebAccess();
	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LastFM::sl_scrobble_response);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LastFM::sl_scrobble_error);

	time_t rawtime, started;
	rawtime = time(nullptr);
	struct tm* ptm = localtime(&rawtime);
	started = mktime(ptm);

	QString artist = metadata.artist;
	QString title = metadata.title;

	UrlParams sig_data;
	sig_data["api_key"] = LFM_API_KEY;
	sig_data["artist"] = artist.toLocal8Bit();
	sig_data["duration"] = QString::number(metadata.length_ms / 1000).toLocal8Bit();
	sig_data["method"] = "track.scrobble";
	sig_data["sk"] = _m->session_key.toLocal8Bit();
	sig_data["timestamp"] = QString::number(started).toLocal8Bit();
	sig_data["track"] = title.toLocal8Bit();

	sig_data.append_signature();

	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post("http://ws.audioscrobbler.com/2.0/", sig_data, post_data);

	lfm_wa->call_post_url(url, post_data);
}


// private slot
void LastFM::sl_similar_artists_available(IDList artist_ids) {

	if(artist_ids.isEmpty()){
		return;
	}

	DatabaseConnector* db = DatabaseConnector::getInstance();
	PlaylistHandler* plh = PlaylistHandler::getInstance();

	PlaylistConstPtr active_playlist;

	int active_idx;

	active_idx = plh->get_active_idx();
	active_playlist = plh->get_playlist_at(active_idx);
	const MetaDataList& v_md = active_playlist->get_playlist();

	std::random_shuffle(artist_ids.begin(), artist_ids.end());

	for( auto it=artist_ids.begin(); it != artist_ids.end(); it++ )
	{
		MetaDataList artist_tracks;

		db->getAllTracksByArtist(*it, artist_tracks);

		std::random_shuffle(artist_tracks.begin(), artist_tracks.end());

		// try all songs of artist
		for(int rounds=0; rounds < artist_tracks.size(); rounds++) {

			int rnd_track = RandomGenerator::get_random_number(0, artist_tracks.size()- 1);

			MetaData md = artist_tracks.takeAt(rnd_track);

			// two times the same track is not allowed
			bool track_exists = std::any_of(v_md.begin(), v_md.end(), [md](const MetaData& it_md){
				return (md.id == it_md.id);
			});

			if(!track_exists){

				MetaDataList v_md; v_md << md;

				plh->append_tracks(v_md, active_idx);
				return;
			}
		}
	}
}

void LastFM::sl_scrobble_response(const QByteArray& data){
	Q_UNUSED(data)
}

void LastFM::sl_scrobble_error(const QString& error){
	sp_log(Log::Warning, "LastFM") << "Scrobble: " << error;
}
