/* StdPlaylist.cpp */

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

#include "StdPlaylist.h"
#include "Helper/globals.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/PlaylistParser.h"
#include "Helper/Random/RandomGenerator.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Set.h"

#include <algorithm>

StdPlaylist::StdPlaylist(int idx, QString name) :
	AbstractPlaylist(idx, name)
{
	_is_storable = true;
}

StdPlaylist::~StdPlaylist() {}

Playlist::Type StdPlaylist::get_type() const
{
	return Playlist::Type::Std;
}


int StdPlaylist::create_playlist(const MetaDataList& v_md) 
{
	bool was_changed = (!metadata().isEmpty() || this->was_changed());

	if(Playlist::Mode::isActiveAndEnabled(_playlist_mode.append())){
		metadata() << v_md;
	}

	else{
		metadata() = v_md;
	}

	set_changed(was_changed);

	return metadata().size();
}



bool StdPlaylist::change_track(int idx) 
{
	metadata().set_cur_play_track(idx);

	// ERROR: invalid idx
	if( !between(idx, metadata()) ) {
		stop();
		return false;
	}

	metadata(idx).played = true;

	// ERROR: track not available in file system anymore
	if( !Helper::File::check_file(metadata(idx).filepath()) ) {
		metadata(idx).is_disabled = true;

		return change_track(idx + 1);
	}

	return true;
}


void StdPlaylist::play() 
{
	if( metadata().isEmpty() ) {
		stop();
		return;
	}

	if(metadata().get_cur_play_track() == -1) {
		metadata().set_cur_play_track(0);
	}
}


void StdPlaylist::pause() {}


void StdPlaylist::stop() 
{
	metadata().set_cur_play_track(-1);

	for(MetaData& md : metadata()){
		md.played = false;
	}
}


void StdPlaylist::fwd() 
{
	Playlist::Mode::State rep1 = _playlist_mode.rep1();
	_playlist_mode.setRep1(false);

	next();

	_playlist_mode.setRep1(rep1);
}


void StdPlaylist::bwd() 
{
	int cur_idx = metadata().get_cur_play_track();
	change_track( cur_idx - 1 );
}


void StdPlaylist::next() 
{
	int cur_track = metadata().get_cur_play_track();
	int track_num = -1;

	// no track
	if(metadata().isEmpty() ) {
		stop();
		return;
	}

	// stopped
	if(track_num == -1){
		track_num = 0;
	}

	if(Playlist::Mode::isActiveAndEnabled(_playlist_mode.rep1())){
		track_num = cur_track;
	}

	// shuffle mode
	else if(Playlist::Mode::isActiveAndEnabled(_playlist_mode.shuffle())) {
		track_num = calc_shuffle_track();
		if(track_num == -1){
			stop();
			return;
		}
	}

	// normal track
	else {
		// last track
		if(cur_track == metadata().size() - 1){
			if(Playlist::Mode::isActiveAndEnabled(_playlist_mode.repAll())){
				track_num = 0;
			}

			else {
				stop();
				return;
			}
		}

		else{
			track_num = cur_track + 1;
		}
	}

	change_track(track_num);
}


int StdPlaylist::calc_shuffle_track()
{
	if(metadata().size() <= 1){
		return -1;
	}

	QList<int> left_tracks;
	int i, left_tracks_idx;
	RandomGenerator rnd;

	// check all tracks played
	i=0;
	for(MetaData& md : metadata()){
		if(!md.played){
			left_tracks << i;
		}

		i++;
	}

	// no random track to play
	if(left_tracks.isEmpty()){
		if(Playlist::Mode::isActiveAndEnabled(_playlist_mode.repAll())){
			return rnd.get_number(0, metadata().size() -1);
		}

		else{
			return -1;
		}
	}

	left_tracks_idx = rnd.get_number(0, left_tracks.size() - 1);

	return left_tracks[left_tracks_idx];
}

void StdPlaylist::metadata_deleted(const MetaDataList& v_md_deleted)
{
	SP::Set<int> indexes;
	for(int i=0; i<metadata().size(); i++){
		for(auto it=v_md_deleted.begin(); it != v_md_deleted.end(); it++){
			if(it->is_equal(metadata()[i])){
				indexes.insert(i);
				break;
			}
		}
	}

	metadata().remove_tracks(indexes);
	emit sig_data_changed(_playlist_idx);
}


void StdPlaylist::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) 
{
	Q_UNUSED(v_md_old)

	for(auto it=metadata().begin(); it !=metadata().end(); it++){
		auto lambda = [it](const MetaData& md) -> bool{
			return it->is_equal(md);
		};

		// find the value in old values
		auto tmp_it = std::find_if(v_md_new.begin(), v_md_new.end(), lambda);
		if(tmp_it != v_md_new.end()){
			// found, overwrite
			*it = *tmp_it;
		}
	}

	emit sig_data_changed(_playlist_idx);
}


void StdPlaylist::metadata_changed_single(const MetaData& md)
{
	IdxList idx_list = find_tracks(md.filepath());

	for(int i : idx_list){
		replace_track(i, md);
	}
}

