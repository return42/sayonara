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
#include "Utils/FileUtils.h"
#include "Utils/Set.h"
#include "Utils/globals.h"
#include "Utils/Settings/Settings.h"
#include "Utils/MetaData/MetaDataList.h"

#include "Components/PlayManager/PlayManager.h"
#include "Components/Tagging/ChangeNotifier.h"

#include <utility>
#include <algorithm>
#include <memory>

using Playlist::Base;

struct Base::Private
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

Base::Base(int idx, const QString& name) :
	Playlist::DBInterface(name),
	SayonaraClass()
{
	Tagging::ChangeNotifier* md_change_notifier = Tagging::ChangeNotifier::instance();
	PlayManagerPtr play_manager = PlayManager::instance();

	m = Pimpl::make<Base::Private>(idx,  _settings->get(Set::PL_Mode));

	connect(md_change_notifier, &Tagging::ChangeNotifier::sig_metadata_changed, this, &Base::metadata_changed);
	connect(md_change_notifier, &Tagging::ChangeNotifier::sig_metadata_deleted, this, &Base::metadata_deleted);

	connect(play_manager, &PlayManager::sig_md_changed, this, &Base::metadata_changed_single);
	connect(play_manager, &PlayManager::sig_duration_changed, this, &Base::duration_changed);

	Set::listen(Set::PL_Mode, this, &Base::_sl_playlist_mode_changed);
}

Base::~Base() {}

void Base::clear()
{
	if(m->v_md.isEmpty()){
		return;
	}

	m->v_md.clear();

	set_changed(true);
}


void Base::move_tracks(const IndexSet& indexes, int tgt)
{
	m->v_md.move_tracks(indexes, tgt);

	set_changed(true);
}


void Base::copy_tracks(const IndexSet& indexes, int tgt)
{
	m->v_md.copy_tracks(indexes, tgt);
}


void Base::delete_tracks(const IndexSet& indexes)
{
	m->v_md.remove_tracks(indexes);
	set_changed(true);
}


void Base::insert_track(const MetaData& md, int tgt)
{
	MetaDataList v_md(md);
	insert_tracks(v_md, tgt);
}


void Base::insert_tracks(const MetaDataList& lst, int tgt)
{
	m->v_md.insert_tracks(lst, tgt);

	set_changed(true);
}


void Base::append_tracks(const MetaDataList& lst)
{
	int old_size = m->v_md.size();

	m->v_md << lst;

	for(auto it=m->v_md.begin() + old_size; it != m->v_md.end(); it++)
	{
		it->is_disabled = !(Util::File::check_file(it->filepath()));
	}

	set_changed(true);
}

bool Base::change_track(int idx)
{
	m->v_md.set_current_track(idx);

	// ERROR: invalid idx
	if( !between(idx, m->v_md) ) {
		stop();
		return false;
	}

	return true;
}


void Base::replace_track(int idx, const MetaData& md)
{
	if( !between(idx, m->v_md) ) {
		return;
	}

	bool is_playing = m->v_md[idx].pl_playing;

	m->v_md[idx] = md;
	m->v_md[idx].is_disabled = !(Util::File::check_file(md.filepath()));
	m->v_md[idx].pl_playing = is_playing;

	emit sig_data_changed( index() );
}

int Base::index() const
{
	return m->playlist_idx;
}


void Base::set_index(int idx)
{
	m->playlist_idx = idx;
}


void Base::set_mode(const Playlist::Mode& mode)
{
	if( m->playlist_mode.shuffle() != mode.shuffle()){
		for(MetaData& md : m->v_md){
			md.played = false;
		}
	}

	m->playlist_mode = mode;
}


uint64_t Base::running_time() const
{
	uint64_t dur_ms = 0;
	dur_ms = std::accumulate(m->v_md.begin(), m->v_md.end(), dur_ms, [](uint64_t time, const MetaData& md){
		return time + md.length_ms;
	});

	return dur_ms;
}

Playlist::Mode Base::mode() const
{
	return m->playlist_mode;
}


int Base::current_track_index() const
{
	return m->v_md.current_track();
}


bool Base::current_track(MetaData &md) const
{
	int cur_play_idx = m->v_md.current_track();

	if(cur_play_idx < 0){
		return false;
	}

	md = m->v_md[cur_play_idx];
	return true;
}


QStringList Base::toStringList() const
{
	return m->v_md.toStringList();
}


IdxList Base::find_tracks(int idx) const
{
	return m->v_md.findTracks(idx);
}


IdxList Base::find_tracks(const QString& filepath) const
{
	return m->v_md.findTracks(filepath);
}


int Base::count() const
{
	return m->v_md.size();
}


bool Base::is_empty() const
{
	return m->v_md.isEmpty();
}


const MetaDataList& Base::playlist() const
{
	return m->v_md;
}


void Base::set_changed(bool b)
{
	m->playlist_changed = b;

	emit sig_data_changed(m->playlist_idx);
}


bool Base::was_changed() const
{
	return m->playlist_changed;
}


bool Base::is_storable() const
{
	return m->is_storable;
}

void Base::set_storable(bool b)
{
	m->is_storable = b;
}


void Base::_sl_playlist_mode_changed()
{
	Playlist::Mode mode = _settings->get(Set::PL_Mode);
	set_mode(mode);
}


const MetaData& Base::operator[](int idx) const
{
	return m->v_md[idx];
}

const MetaDataList& Base::metadata() const
{
	return m->v_md;
}

const MetaData& Base::metadata(int idx) const
{
	return m->v_md[idx];
}


MetaDataList& Base::metadata()
{
	return m->v_md;
}


MetaData& Base::metadata(int idx)
{
	return m->v_md[idx];
}

