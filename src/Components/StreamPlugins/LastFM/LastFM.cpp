/* LastFM.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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
#include "Database/LibraryDatabase.h"

#include <QDomDocument>
#include <QUrl>

#include <algorithm>
#include <ctime>

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

	uint64_t						old_pos;
	uint64_t						old_pos_difference;

	MetaData					md;
};

LastFM::LastFM() :
	QObject(),
	SayonaraClass()
{
	m = Pimpl::make<LastFM::Private>();
	m->play_manager = PlayManager::getInstance();

	m->logged_in = false;
	m->track_changed_thread = new LFMTrackChangedThread("", "", this);
	m->login_thread = new LFMLoginThread(this);

	connect(m->login_thread, &LFMLoginThread::sig_logged_in, this, &LastFM::sl_login_thread_finished);
	connect(m->play_manager, &PlayManager::sig_track_changed,	this, &LastFM::sl_track_changed);
	connect(m->play_manager, &PlayManager::sig_position_changed_ms, this, &LastFM::sl_position_ms_changed);
	connect(m->track_changed_thread, &LFMTrackChangedThread::sig_similar_artists_available,
			this, &LastFM::sl_similar_artists_available);

	REGISTER_LISTENER_NO_CALL(Set::LFM_Login, psl_login);
	REGISTER_LISTENER(Set::LFM_Active, psl_login);
}

LastFM::~LastFM() {}

void LastFM::get_login(QString& user, QString& pw)
{
	StringPair user_pw = Settings::getInstance()->get(Set::LFM_Login);
	user = user_pw.first;
	pw = user_pw.second;
}


bool LastFM::is_logged_in()
{
	return m->logged_in;
}


void LastFM::psl_login()
{
	m->active = _settings->get(Set::LFM_Active);
	if(!m->active){
		return;
	}

	QString password;
	get_login(m->username, password);

	m->logged_in = false;
	m->login_thread->login(m->username, password);
}


void LastFM::sl_login_thread_finished(bool success)
{
	if(!success){
		return;
	}

	LFMLoginStuff login_info = m->login_thread->getLoginStuff();

	m->logged_in = login_info.logged_in;
	m->auth_token = login_info.token;
	m->session_key = login_info.session_key;

	_settings->set(Set::LFM_SessionKey, m->session_key);

	sp_log(Log::Debug, this) << "Got session key";

	if(!m->logged_in){
		sp_log(Log::Warning, this) << "Cannot login";
	}

	emit sig_logged_in(m->logged_in);
}

void LastFM::sl_track_changed(const MetaData& md)
{
	Playlist::Mode pl_mode = _settings->get(Set::PL_Mode);
	if( Playlist::Mode::isActiveAndEnabled(pl_mode.dynamic())) {
		m->track_changed_thread->search_similar_artists(md);
	}

	if(!m->active ) return;
	if(!m->logged_in ) return;

	m->md = md;

	m->track_changed_thread->set_username(m->username);
	m->track_changed_thread->set_session_key(m->session_key);

	reset_scrobble();

	m->track_changed_thread->update_now_playing(md);
}


void LastFM::sl_position_ms_changed(uint64_t pos_ms)
{
	if(!m->active){
		return;
	}

	check_scrobble(pos_ms);
}


void LastFM::reset_scrobble()
{
	m->scrobbled = false;
	m->old_pos = 0;
	m->old_pos_difference = 0;
}


bool LastFM::check_scrobble(uint64_t pos_ms)
{
	if(!m->logged_in){
		return false;
	}

	if(m->scrobbled){
		return false;
	}

	if(m->md.length_ms <= 0){
		return false;
	}

	if(m->old_pos == 0){
		m->old_pos = pos_ms;
		m->old_pos_difference = 0;
		return false;
	}

	else{
		if(m->old_pos > pos_ms){
			m->old_pos = 0;
		}

		else if(pos_ms > m->old_pos + 2000){
			m->old_pos = 0;
		}

		else{
			uint64_t scrobble_time_ms = (uint64_t) (_settings->get(Set::LFM_ScrobbleTimeSec) * 1000);

			m->old_pos_difference += (pos_ms - m->old_pos);
			m->old_pos = pos_ms;

			if( (m->old_pos_difference > scrobble_time_ms) ||
				(m->old_pos_difference >= ((m->md.length_ms  * 3) / 4) && m->md.length_ms >= 1000))
			{
				scrobble(m->md);
			}
		}
	}

	return m->scrobbled;
}

void LastFM::scrobble(const MetaData& md)
{
	m->scrobbled = true;

	if(!m->active) {
		return;
	}

	if(!m->logged_in){
		return;
	}

	LFMWebAccess* lfm_wa = new LFMWebAccess();
	connect(lfm_wa, &LFMWebAccess::sig_response, this, &LastFM::sl_scrobble_response);
	connect(lfm_wa, &LFMWebAccess::sig_error, this, &LastFM::sl_scrobble_error);

	time_t rawtime, started;
	rawtime = time(nullptr);
	struct tm* ptm = localtime(&rawtime);
	started = mktime(ptm);

	QString artist = md.artist;
	QString title = md.title;

	UrlParams sig_data;
	sig_data["api_key"] = LFM_API_KEY;
	sig_data["artist"] = artist.toLocal8Bit();
	sig_data["duration"] = QString::number(md.length_ms / 1000).toLocal8Bit();
	sig_data["method"] = "track.scrobble";
	sig_data["sk"] = m->session_key.toLocal8Bit();
	sig_data["timestamp"] = QString::number(started).toLocal8Bit();
	sig_data["track"] = title.toLocal8Bit();

	sig_data.append_signature();

	QByteArray post_data;
	QString url = lfm_wa->create_std_url_post("http://ws.audioscrobbler.com/2.0/", sig_data, post_data);

	lfm_wa->call_post_url(url, post_data);
}


// private slot
void LastFM::sl_similar_artists_available(IDList artist_ids)
{
	return;
	if(artist_ids.isEmpty()){
		return;
	}

	DatabaseConnector* db = DatabaseConnector::getInstance();
	LibraryDatabase* lib_db = db->library_db(-1, 0);

	PlaylistHandler* plh = PlaylistHandler::getInstance();

	PlaylistConstPtr active_playlist;

	int active_idx;

	active_idx = plh->get_active_idx();
	active_playlist = plh->get_playlist_at(active_idx);
	const MetaDataList& v_md = active_playlist->playlist();

	std::random_shuffle(artist_ids.begin(), artist_ids.end());

	for( auto it=artist_ids.begin(); it != artist_ids.end(); it++ )
	{
		MetaDataList artist_tracks;
		lib_db->getAllTracksByArtist(*it, artist_tracks);

		std::random_shuffle(artist_tracks.begin(), artist_tracks.end());

		// try all songs of artist
		for(int rounds=0; rounds < artist_tracks.count(); rounds++) 
		{
			int rnd_track = RandomGenerator::get_random_number(0, artist_tracks.size()- 1);

			MetaData md = artist_tracks.take_at(rnd_track);

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
	sp_log(Log::Warning, this) << "Scrobble: " << error;
}
