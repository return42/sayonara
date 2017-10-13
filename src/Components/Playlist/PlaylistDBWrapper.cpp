/* PlaylistDBWrapper.cpp */

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

#include "PlaylistDBWrapper.h"
#include "Utils/Tagging/Tagging.h"
#include "Utils/FileUtils.h"
#include "Utils/Parser/PlaylistParser.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Playlist/CustomPlaylist.h"
#include "Database/DatabaseConnector.h"

#include <utility>

struct PlaylistDBWrapper::Private
{
    DatabaseConnector* db=nullptr;

    Private()
    {
        db = DatabaseConnector::instance();
    }
};

PlaylistDBWrapper::PlaylistDBWrapper()
{
    m = Pimpl::make<Private>();
}


PlaylistDBWrapper::~PlaylistDBWrapper() {}


void PlaylistDBWrapper::apply_tags(MetaDataList& v_md)
{
    for(MetaData& md : v_md)
    {
        if(md.is_extern)
        {
			if(Util::File::is_file(md.filepath())){
                Tagging::Util::getMetaDataOfFile(md);
			}
		}
	}
}


bool PlaylistDBWrapper::get_skeletons(CustomPlaylistSkeletons& skeletons, DatabasePlaylist::PlaylistChooserType type, Playlist::SortOrder so){
    return m->db->getAllPlaylistSkeletons(skeletons, type, so);
}

bool PlaylistDBWrapper::get_all_skeletons(CustomPlaylistSkeletons& skeletons,
					   Playlist::SortOrder so)
{
	return get_skeletons(skeletons,
						 DatabaseConnector::PlaylistChooserType::TemporaryAndPermanent,
						 so);
}

bool PlaylistDBWrapper::get_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
                                                Playlist::SortOrder so)
{
	return get_skeletons(skeletons,
	                     DatabaseConnector::PlaylistChooserType::OnlyTemporary,
	                     so);
}

bool PlaylistDBWrapper:: get_non_temporary_skeletons(CustomPlaylistSkeletons& skeletons,
													 Playlist::SortOrder so)
{
	return get_skeletons(skeletons,
						 DatabaseConnector::PlaylistChooserType::OnlyPermanent,
						 so);
}


bool PlaylistDBWrapper::get_playlists(CustomPlaylists& playlists, DatabasePlaylist::PlaylistChooserType type, Playlist::SortOrder so)
{
	Q_UNUSED(type)

	bool success;
	CustomPlaylistSkeletons skeletons;

	success = get_all_skeletons(skeletons, so);
	if(!success){
		return false;
	}

	bool load_temporary = (type == DatabasePlaylist::PlaylistChooserType::OnlyTemporary ||
	                       type == DatabasePlaylist::PlaylistChooserType::TemporaryAndPermanent);

	bool load_permanent = (type == DatabasePlaylist::PlaylistChooserType::OnlyPermanent ||
	                       type == DatabasePlaylist::PlaylistChooserType::TemporaryAndPermanent);

	for(const CustomPlaylistSkeleton& skeleton : skeletons){
		CustomPlaylist pl(skeleton);
		if(pl.id() < 0){
			continue;
		}

        success = m->db->getPlaylistById(pl);

		if(!success){
			continue;
		}

		apply_tags(pl);

		if( (pl.temporary() && load_temporary) ||
			(!pl.temporary() && load_permanent) )
		{
			playlists.push_back(pl);
		}
	}

	return true;
}


bool PlaylistDBWrapper::get_all_playlists(CustomPlaylists& playlists, Playlist::SortOrder so)
{
	return get_playlists(playlists,
						 DatabaseConnector::PlaylistChooserType::TemporaryAndPermanent,
						 so);
}


bool PlaylistDBWrapper::get_temporary_playlists(CustomPlaylists& playlists, Playlist::SortOrder so)
{
	return get_playlists(playlists,
						 DatabaseConnector::PlaylistChooserType::OnlyTemporary,
						 so);
}


bool PlaylistDBWrapper::get_non_temporary_playlists(CustomPlaylists& playlists, Playlist::SortOrder so)
{
	return get_playlists(playlists,
						 DatabaseConnector::PlaylistChooserType::OnlyPermanent,
						 so);
}


CustomPlaylist PlaylistDBWrapper::get_playlist_by_id(int id)
{
	bool success;
	CustomPlaylist pl;
	pl.set_id(id);

    success = m->db->getPlaylistById(pl);
	if(!success){
		return pl;
	}

	return pl;
}


CustomPlaylist PlaylistDBWrapper::get_playlist_by_name(const QString& name)
{
    int id = m->db->getPlaylistIdByName(name);

	if(id < 0){
		CustomPlaylist pl;
		pl.set_id(-1);
		return pl;
	}

	return get_playlist_by_id(id);
}

bool PlaylistDBWrapper::rename_playlist(int id, const QString& new_name)
{
    return m->db->renamePlaylist(id, new_name);
}


bool PlaylistDBWrapper::save_playlist_as(const MetaDataList& v_md, const QString& name)
{
	bool success;

    m->db->transaction();
    success = m->db->storePlaylist(v_md, name, false);
    m->db->commit();

	return success;
}

bool PlaylistDBWrapper::save_playlist_temporary(const MetaDataList& v_md, const QString& name)
{
	bool success;

    m->db->transaction();

    success = m->db->storePlaylist(v_md, name, true);
    m->db->commit();

	return success;
}


bool PlaylistDBWrapper::save_playlist(const CustomPlaylist& pl)
{
	bool success;

    m->db->transaction();
	// TODO! we dont need the two other parameters
    success = m->db->storePlaylist(pl, pl.id(), pl.temporary());
    m->db->commit();

	return success;
}


bool PlaylistDBWrapper::save_playlist(const MetaDataList& v_md, int id, bool is_temporary)
{
	bool success;

    m->db->transaction();
	// TODO: see above
    success = m->db->storePlaylist(v_md, id, is_temporary);
    m->db->commit();

	return success;
}


bool PlaylistDBWrapper::delete_playlist(int id)
{
    return m->db->deletePlaylist(id);
}


bool PlaylistDBWrapper::delete_playlist(const QString& name)
{
    int id = m->db->getPlaylistIdByName(name);
    return m->db->deletePlaylist(id);
}


bool PlaylistDBWrapper::exists(const QString& name)
{
    int id = m->db->getPlaylistIdByName(name);
	return (id >= 0);
}
