/* Playlist.cpp */

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

#include "AbstractPlaylist.h"
#include "Helper/FileHelper.h"
#include "Helper/Set.h"
#include "Helper/globals.h"
#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/MetaDataList.h"

#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Components/Engine/EngineHandler.h"

#include <utility>
#include <algorithm>
#include <memory>

struct AbstractPlaylist::Private
{
	MetaDataList    v_md;
	bool			playlist_changed;

	Private()
	{
		playlist_changed = false;
	}
};

AbstractPlaylist::AbstractPlaylist(int idx, const QString& name) :
	PlaylistDBInterface(name),
	SayonaraClass()
{
	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	EngineHandler* engine = EngineHandler::getInstance();

	_m = Pimpl::make<AbstractPlaylist::Private>();

	_playlist_idx = idx;
	_playlist_mode = _settings->get(Set::PL_Mode);
	_is_storable = false;

	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed, this, &AbstractPlaylist::metadata_changed);
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_deleted, this, &AbstractPlaylist::metadata_deleted);

	connect(engine, &EngineHandler::sig_md_changed, this, &AbstractPlaylist::metadata_changed_single);
	connect(engine, &EngineHandler::sig_dur_changed, this, &AbstractPlaylist::metadata_changed_single);

	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
}

AbstractPlaylist::~AbstractPlaylist() {}

void AbstractPlaylist::clear() 
{
	if(_m->v_md.isEmpty()){
		return;
	}

	_m->v_md.clear();

	set_changed(true);
}


void AbstractPlaylist::move_tracks(const SP::Set<int>& indexes, int tgt) 
{
	_m->v_md.move_tracks(indexes, tgt);

	set_changed(true);
}


void AbstractPlaylist::copy_tracks(const SP::Set<int>& indexes, int tgt) 
{
	_m->v_md.copy_tracks(indexes, tgt);
}


void AbstractPlaylist::delete_tracks(const SP::Set<int>& indexes) 
{
	_m->v_md.remove_tracks(indexes);
	set_changed(true);
}


void AbstractPlaylist::insert_track(const MetaData& md, int tgt) 
{
	MetaDataList v_md;
	v_md << md;
	insert_tracks(v_md, tgt);
}


void AbstractPlaylist::insert_tracks(const MetaDataList& lst, int tgt) 
{
	_m->v_md.insert_tracks(lst, tgt);

	set_changed(true);
}


void AbstractPlaylist::append_tracks(const MetaDataList& lst) 
{
	for(const MetaData& md : lst){
		_m->v_md << std::move(md);
		_m->v_md.last().is_disabled = !(Helper::File::check_file(md.filepath()));
	}

	set_changed(true);
}


void AbstractPlaylist::replace_track(int idx, const MetaData& md) 
{
	if( !between(idx, _m->v_md) ) {
		return;
	}

	bool is_playing = _m->v_md[idx].pl_playing;

	_m->v_md[idx] = md;
	_m->v_md[idx].is_disabled = !(Helper::File::check_file(md.filepath()));
	_m->v_md[idx].pl_playing = is_playing;

	emit sig_data_changed(_playlist_idx);
}


MetaDataList& AbstractPlaylist::metadata()
{
	return _m->v_md;
}


MetaData& AbstractPlaylist::metadata(int i)
{
	return _m->v_md[i];
}


int AbstractPlaylist::get_idx() const
{
	return _playlist_idx;
}


void AbstractPlaylist::set_idx(int idx){
	_playlist_idx = idx;
}


void AbstractPlaylist::set_playlist_mode(const Playlist::Mode& mode)
{
	if(_playlist_mode.shuffle() != mode.shuffle()){
		for(MetaData& md : _m->v_md){
			md.played = false;
		}
	}

	_playlist_mode = mode;
}


quint64 AbstractPlaylist::get_running_time() const
{
	quint64 dur_ms = 0;
	dur_ms = std::accumulate(_m->v_md.begin(), _m->v_md.end(), dur_ms, [](quint64 time, const MetaData& md){
		return time + md.length_ms;
	});

	return dur_ms;
}


int AbstractPlaylist::get_cur_track_idx() const 
{
	return _m->v_md.current_track();
}


bool AbstractPlaylist::get_cur_track(MetaData &md) const 
{
	int cur_play_idx = _m->v_md.current_track();

	if(cur_play_idx < 0){
		return false;
	}

	md = _m->v_md[cur_play_idx];
	return true;
}


QStringList AbstractPlaylist::toStringList() const 
{
	return _m->v_md.toStringList();
}


IdxList AbstractPlaylist::find_tracks(int idx) const 
{
	return _m->v_md.findTracks(idx);
}


IdxList AbstractPlaylist::find_tracks(const QString& filepath) const 
{
	return _m->v_md.findTracks(filepath);
}


int AbstractPlaylist::get_count() const 
{
	return _m->v_md.size();
}


bool AbstractPlaylist::is_empty() const 
{
	return _m->v_md.isEmpty();
}


const MetaDataList& AbstractPlaylist::get_playlist() const
{
	return _m->v_md;
}


void AbstractPlaylist::set_changed(bool b)
{
	_m->playlist_changed = b;

	emit sig_data_changed(_playlist_idx);
}


bool AbstractPlaylist::was_changed() const
{
	return _m->playlist_changed;
}


bool AbstractPlaylist::is_storable() const
{
	return _is_storable;
}


void AbstractPlaylist::_sl_playlist_mode_changed()
{
	Playlist::Mode mode = _settings->get(Set::PL_Mode);
	set_playlist_mode(mode);
}


const MetaData& AbstractPlaylist::operator[](int idx) const
{
	return _m->v_md[idx];
}

const MetaData& AbstractPlaylist::at_const_ref(int idx) const 
{
	return _m->v_md[idx];
}
