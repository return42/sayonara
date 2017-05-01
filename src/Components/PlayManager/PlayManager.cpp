/* PlayManager.cpp */

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

#include "PlayManager.h"
#include "Interfaces/Notification/NotificationHandler.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QDateTime>
#include <QTime>
#include <algorithm>


template<typename T, int N_ITEMS>
class RingBuffer {
	private:
		int _cur_idx;
		int _n_items;
		T _data[N_ITEMS];

	public:
		RingBuffer()
		{
			clear();
		}

		void clear()
		{
			_cur_idx = 0;
			_n_items = 0;
		}

		void insert(const T& item){
			_data[_cur_idx] = item;
			_cur_idx = (_cur_idx + 1) % N_ITEMS;
			_n_items = std::min(N_ITEMS, _n_items + 1);
		}

		bool has_item(const T& item) const
		{
			for(int i=0; i<_n_items; i++){
				if(_data[i] == item){
					return true;
				}
			}

			return false;
		}

		int count() const
		{
			return _n_items;
		}

		bool is_empty() const
		{
			return (_n_items == 0);
		}
};


struct PlayManager::Private
{
		MetaData				md;
		RingBuffer<QString, 3>	ring_buffer;
		quint64					position_ms;
		int						cur_idx;
		quint64					initial_position_ms;
		PlayState				playstate;

		Private()
		{
			initial_position_ms = 0;
			position_ms = 0;
			cur_idx = -1;
			playstate = PlayState::Stopped;
		}
};


PlayManager::PlayManager(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_m = Pimpl::make<Private>();

	bool load_playlist = (_settings->get(Set::PL_LoadSavedPlaylists) || _settings->get(Set::PL_LoadTemporaryPlaylists));
	bool load_last_track = _settings->get(Set::PL_LoadLastTrack);
	bool remember_last_time = _settings->get(Set::PL_RememberTime);

	if(load_playlist && load_last_track && remember_last_time){
		_m->initial_position_ms = _settings->get(Set::Engine_CurTrackPos_s) * 1000;
	}

	else{
		_m->initial_position_ms = 0;
	}

	stop();
}

PlayManager::~PlayManager()
{
	_settings->set(Set::Engine_CurTrackPos_s, (int) (_m->position_ms / 1000));
}

PlayState PlayManager::get_play_state() const
{
	return _m->playstate;
}

quint64 PlayManager::get_cur_position_ms() const
{
	return _m->position_ms;
}

quint64 PlayManager::get_init_position_ms() const
{
	return _m->initial_position_ms;
}

quint64 PlayManager::get_duration_ms() const
{
	return _m->md.length_ms;
}

MetaData PlayManager::get_cur_track() const
{
	return _m->md;
}

int PlayManager::get_volume() const
{
	return _settings->get(Set::Engine_Vol);
}

bool PlayManager::get_mute() const
{
	return _settings->get(Set::Engine_Mute);
}

void PlayManager::play()
{
	if(_m->playstate == PlayState::Stopped && _m->cur_idx == -1){
		_m->playstate = PlayState::Playing;
		next();
		return;
	}

	_m->playstate = PlayState::Playing;

	emit sig_playstate_changed(_m->playstate);
}


void PlayManager::play_pause()
{
	if(_m->playstate == PlayState::Playing){
		pause();
	}

	else{
		play();
	}
}


void PlayManager::pause()
{
	if(_m->playstate == PlayState::Stopped){
		_m->playstate = PlayState::Paused;
		next();
		return;
	}

	_m->playstate = PlayState::Paused;

	emit sig_playstate_changed(_m->playstate);
}


void PlayManager::previous()
{
	emit sig_previous();
}


void PlayManager::next()
{
	emit sig_next();
}


void PlayManager::stop()
{
	_m->md = MetaData();
	_m->ring_buffer.clear();
	_m->cur_idx = -1;
	_m->playstate = PlayState::Stopped;

	emit sig_playstate_changed(_m->playstate);
}


void PlayManager::record(bool b)
{
	if(_settings->get(SetNoDB::MP3enc_found)){
		emit sig_record(b);
	} else {
		emit sig_record(false);
	}
}

void PlayManager::seek_rel(double percent)
{
	emit sig_seeked_rel(percent);
}

void PlayManager::seek_rel_ms(qint64 ms)
{
	emit sig_seeked_rel_ms(ms);
}

void PlayManager::seek_abs_ms(quint64 ms)
{
	emit sig_seeked_abs_ms(ms);
}

void PlayManager::set_position_ms(quint64 ms)
{
	_m->position_ms = ms;

	if(_m->position_ms % 1000 == 0){
		_settings->set(Set::Engine_CurTrackPos_s, (int) (_m->position_ms / 1000));
	}

	emit sig_position_changed_ms(ms);
}


void PlayManager::change_track(const MetaData& md, int playlist_idx)
{
	_m->md = md;
	_m->position_ms = 0;
	_m->cur_idx = playlist_idx;
	_m->ring_buffer.clear();

	// initial position is outdated now and never needed again
	if(_m->initial_position_ms > 0){
		int old_idx = _settings->get(Set::PL_LastTrack);
		if(old_idx != _m->cur_idx){
			_m->initial_position_ms = 0;
		}
	}

	// play or stop
	if(_m->cur_idx >= 0){
		emit sig_track_changed(_m->md);
		emit sig_track_idx_changed(_m->cur_idx);

		play();
		if( (md.radio_mode() != RadioMode::Off) &&
			_settings->get(Set::Engine_SR_Active) &&
			_settings->get(Set::Engine_SR_AutoRecord) )
		{
			record(true);
		}
	}

	else {
		sp_log(Log::Info) << "Playlist finished";
		emit sig_playlist_finished();
		stop();
	}

	// save last track
	if(md.db_id == 0){
		_settings->set(Set::PL_LastTrack, _m->cur_idx);
	}

	else{
		_settings->set(Set::PL_LastTrack, -1);
	}

	// show notification
	if(_settings->get(Set::Notification_Show)){
		if(_m->cur_idx > -1 && !_m->md.filepath().isEmpty()){
			NotificationHandler::getInstance()->notify(_m->md);
		}
	}
}


void PlayManager::set_track_ready()
{
	if(_m->initial_position_ms > 0){
		sp_log(Log::Debug, this) << "Track ready, " << (int) (_m->initial_position_ms / 1000);
		this->seek_abs_ms(_m->initial_position_ms);
		_m->initial_position_ms = 0;
	}
}

void PlayManager::buffering(int progress)
{
	emit sig_buffer(progress);
}


void PlayManager::volume_up()
{
	set_volume(_settings->get(Set::Engine_Vol) + 5);
}

void PlayManager::volume_down()
{
	set_volume(_settings->get(Set::Engine_Vol) - 5);
}

void PlayManager::set_volume(int vol)
{
	vol = std::min(vol, 100);
	vol = std::max(vol, 0);
	_settings->set(Set::Engine_Vol, vol);
	emit sig_volume_changed(vol);
}

void PlayManager::set_mute(bool b)
{
	_settings->set(Set::Engine_Mute, b);
	emit sig_mute_changed(b);
}

void PlayManager::change_duration(qint64 ms){
	_m->md.length_ms = ms;

	emit sig_duration_changed(ms);
}

void PlayManager::change_metadata(const MetaData& md)
{
	MetaData md_old = _m->md;
	_m->md = md;

	QString str = md.title + md.artist + md.album;
	bool has_data = _m->ring_buffer.has_item(str);

	if(!has_data){
		if(_settings->get(Set::Notification_Show)){
			NotificationHandler::getInstance()->notify(_m->md);
		}

		if( _m->ring_buffer.count() > 0 ) {
			md_old.album = "";
			md_old.is_disabled = true;
			md_old.set_filepath("");

			QDateTime date = QDateTime::currentDateTime();
			QTime time = date.time();
			md_old.length_ms = (time.hour() * 60 + time.minute()) * 1000;

			emit sig_www_track_finished(md_old);
		}

		_m->ring_buffer.insert(str);
	}

	emit sig_md_changed(md);
}
