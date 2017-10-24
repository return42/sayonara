/* PlaylistChooser.cpp */

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

#include "PlaylistChooser.h"
#include "Utils/Playlist/CustomPlaylist.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/Playlist/PlaylistDBWrapper.h"

#include "Utils/Logger/Logger.h"

#include <QStringList>

struct PlaylistChooser::Private
{
    CustomPlaylistSkeletons	skeletons;
    int						import_state;

    PlaylistHandler*		playlist_handler=nullptr;
    PlaylistDBWrapperPtr	playlist_db_connector=nullptr;

    Private()
    {
        playlist_handler = PlaylistHandler::instance();
        playlist_db_connector = std::make_shared<PlaylistDBWrapper>();
    }

    CustomPlaylist find_custom_playlist(int id)
    {
        CustomPlaylist pl = playlist_db_connector->get_playlist_by_id(id);
        return pl;
    }
};

PlaylistChooser::PlaylistChooser()
{
    m = Pimpl::make<Private>();

    connect(m->playlist_handler, &PlaylistHandler::sig_saved_playlists_changed,
			this, &PlaylistChooser::load_all_playlists);
}

PlaylistChooser::~PlaylistChooser() {}

void PlaylistChooser::load_all_playlists()
{
	bool success;

    m->skeletons.clear();
    success = m->playlist_db_connector->get_non_temporary_skeletons(
                m->skeletons, Playlist::SortOrder::NameAsc
    );

	if(success) {
        emit sig_all_playlists_loaded(m->skeletons);
	}
}

void PlaylistChooser::load_single_playlist(int id)
{
	int idx;
	if(id < 0) {
		return;
	}

    CustomPlaylist pl = m->find_custom_playlist(id);
	if(!pl.valid()) {
		return;
	}

    idx = m->playlist_handler->create_playlist(pl);
    m->playlist_handler->set_current_idx(idx);
}


int PlaylistChooser::find_playlist(const QString& name) const
{
    for(const CustomPlaylistSkeleton& skeleton : m->skeletons)
    {
		if(skeleton.name().compare(name) == 0){
			return skeleton.id();
		}
	}

	return -1;
}

