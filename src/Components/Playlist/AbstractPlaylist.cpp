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

#include "Components/PlayManager/PlayManager.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"

#include <utility>
#include <algorithm>
#include <memory>

struct AbstractPlaylist::Private
{
	MetaDataList    v_md;
	int				playlist_idx;
	Playlist::Mode	playlist_mode;
	bool			playlist_changed;
	bool			is_storable;

	Private(int playlist_idx, Playlist::Mode playlist_mode) :
		playlist_idx(playlist_idx),
		playlist_mode(playlist_mode),
		playlist_changed(false),
		is_storable(false)
	{}
};

AbstractPlaylist::AbstractPlaylist(int idx, const QString& name) :
	PlaylistDBInterface(name),
	SayonaraClass()
{
	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	PlayManager* play_manager = PlayManager::getInstance();

    m = Pimpl::make<AbstractPlaylist::Private>(idx,  _settings->get(Set::PL_Mode));

	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed, this, &AbstractPlaylist::metadata_changed);
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_deleted, this, &AbstractPlaylist::metadata_deleted);

	connect(play_manager, &PlayManager::sig_md_changed, this, &AbstractPlaylist::metadata_changed_single);
	connect(play_manager, &PlayManager::sig_duration_changed, this, &AbstractPlaylist::duration_changed);

	Set::listen(Set::PL_Mode, this, &AbstractPlaylist::_sl_playlist_mode_changed);
}

AbstractPlaylist::~AbstractPlaylist() {}

void AbstractPlaylist::clear() 
{
    if(m->v_md.isEmpty()){
		return;
	}

    m->v_md.clear();

	set_changed(true);
}


void AbstractPlaylist::move_tracks(const SP::Set<int>& indexes, int tgt) 
{
    m->v_md.move_tracks(indexes, tgt);

	set_changed(true);
}


void AbstractPlaylist::copy_tracks(const SP::Set<int>& indexes, int tgt) 
{
    m->v_md.copy_tracks(indexes, tgt);
}


void AbstractPlaylist::delete_tracks(const SP::Set<int>& indexes) 
{
    m->v_md.remove_tracks(indexes);
	set_changed(true);
}


void AbstractPlaylist::insert_track(const MetaData& md, int tgt) 
{
	MetaDataList v_md(md);
	insert_tracks(v_md, tgt);
}


void AbstractPlaylist::insert_tracks(const MetaDataList& lst, int tgt) 
{
    m->v_md.insert_tracks(lst, tgt);

	set_changed(true);
}


void AbstractPlaylist::append_tracks(const MetaDataList& lst) 
{
    int old_size = m->v_md.size();
	
    m->v_md << lst;

    for(auto it=m->v_md.begin() + old_size; it != m->v_md.end(); it++)
	{
		it->is_disabled = !(Helper::File::check_file(it->filepath()));
	}

    set_changed(true);
}

bool AbstractPlaylist::change_track(int idx)
{
    m->v_md.set_current_track(idx);

    // ERROR: invalid idx
    if( !between(idx, m->v_md) ) {
        stop();
        return false;
    }

    return true;
}


void AbstractPlaylist::replace_track(int idx, const MetaData& md) 
{
    if( !between(idx, m->v_md) ) {
		return;
	}

    bool is_playing = m->v_md[idx].pl_playing;

    m->v_md[idx] = md;
    m->v_md[idx].is_disabled = !(Helper::File::check_file(md.filepath()));
    m->v_md[idx].pl_playing = is_playing;

	emit sig_data_changed( playlist_index() );
}


MetaDataList& AbstractPlaylist::metadata()
{
    return m->v_md;
}


MetaData& AbstractPlaylist::metadata(int i)
{
    return m->v_md[i];
}


int AbstractPlaylist::playlist_index() const
{
    return m->playlist_idx;
}


void AbstractPlaylist::set_playlist_index(int idx)
{
    m->playlist_idx = idx;
}


void AbstractPlaylist::set_playlist_mode(const Playlist::Mode& mode)
{
    if( m->playlist_mode.shuffle() != mode.shuffle()){
        for(MetaData& md : m->v_md){
			md.played = false;
		}
	}

    m->playlist_mode = mode;
}


uint64_t AbstractPlaylist::running_time() const
{
	uint64_t dur_ms = 0;
    dur_ms = std::accumulate(m->v_md.begin(), m->v_md.end(), dur_ms, [](uint64_t time, const MetaData& md){
		return time + md.length_ms;
	});

	return dur_ms;
}

Playlist::Mode AbstractPlaylist::playlist_mode() const
{
    return m->playlist_mode;
}


int AbstractPlaylist::current_track_index() const
{
    return m->v_md.current_track();
}


bool AbstractPlaylist::current_track(MetaData &md) const
{
    int cur_play_idx = m->v_md.current_track();

	if(cur_play_idx < 0){
		return false;
	}

    md = m->v_md[cur_play_idx];
	return true;
}


QStringList AbstractPlaylist::toStringList() const 
{
    return m->v_md.toStringList();
}


IdxList AbstractPlaylist::find_tracks(int idx) const 
{
    return m->v_md.findTracks(idx);
}


IdxList AbstractPlaylist::find_tracks(const QString& filepath) const 
{
    return m->v_md.findTracks(filepath);
}


int AbstractPlaylist::count() const
{
    return m->v_md.size();
}


bool AbstractPlaylist::is_empty() const 
{
    return m->v_md.isEmpty();
}


const MetaDataList& AbstractPlaylist::playlist() const
{
    return m->v_md;
}


void AbstractPlaylist::set_changed(bool b)
{
    m->playlist_changed = b;

    emit sig_data_changed(m->playlist_idx);
}


bool AbstractPlaylist::was_changed() const
{
    return m->playlist_changed;
}


bool AbstractPlaylist::is_storable() const
{
    return m->is_storable;
}

void AbstractPlaylist::set_storable(bool b)
{
    m->is_storable = b;
}


void AbstractPlaylist::_sl_playlist_mode_changed()
{
	Playlist::Mode mode = _settings->get(Set::PL_Mode);
	set_playlist_mode(mode);
}


const MetaData& AbstractPlaylist::operator[](int idx) const
{
    return m->v_md[idx];
}

const MetaData& AbstractPlaylist::at_const_ref(int idx) const 
{
    return m->v_md[idx];
}
