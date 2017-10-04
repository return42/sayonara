/* LocalLibrary.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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

#include "LocalLibrary.h"
#include "LibraryManager.h"
#include "Importer/LibraryImporter.h"
#include "Threads/ReloadThread.h"
#include "Threads/IndexDirectoriesThread.h"
#include "Database/DatabaseConnector.h"
#include "Database/LocalLibraryDatabase.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/TagEdit/TagEdit.h"

#include "Helper/Settings/Settings.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/Logger/Logger.h"
#include "Helper/globals.h"

#include <utility>
#include <QTime>

struct LocalLibrary::Private
{
	DatabaseConnector*	db=nullptr;
	LibraryDatabase*	lib_db=nullptr;

	ReloadThread* 		reload_thread=nullptr;
	LibraryImporter*	library_importer=nullptr;
	QString				library_path;
	QString				library_name;

    int8_t				lib_id;

	Private(const QString& library_name, const QString& library_path, int8_t lib_id) :
		db(DatabaseConnector::getInstance()),
		lib_db(db->library_db(lib_id, 0)),
		library_path(library_path),
        library_name(library_name),
        lib_id(lib_id)
	{}
};

LocalLibrary::LocalLibrary(int8_t lib_id, const QString& library_name, const QString& library_path, QObject *parent) :
	AbstractLibrary(parent)
{
	DatabaseConnector::getInstance()->register_library_db<LocalLibraryDatabase>(lib_id);

	m = Pimpl::make<Private>(library_name, library_path, lib_id);

	apply_db_fixes();

	connect(_playlist, &PlaylistHandler::sig_track_deletion_requested, this, &LocalLibrary::delete_tracks);

	Set::listen(Set::Lib_SearchMode, this, &LocalLibrary::_sl_search_mode_changed, false);
}

LocalLibrary::~LocalLibrary() {}

void LocalLibrary::clear_library()
{
	m->lib_db->clear();
}

void LocalLibrary::apply_db_fixes() {}


void LocalLibrary::psl_reload_library(bool clear_first, Library::ReloadQuality quality)
{
	if(m->reload_thread && m->reload_thread->is_running()){
		return;
	}

	if(!m->reload_thread){
		init_reload_thread();
	}

	if(clear_first) {
		delete_all_tracks();
	}

	m->reload_thread->set_quality(quality);
	m->reload_thread->set_library(library_id(), library_path());
	m->reload_thread->start();
}


void LocalLibrary::reload_thread_finished()
{
	load();

	emit sig_reloading_library("", -1);
	emit sig_reloading_library_finished();
}

void LocalLibrary::_sl_search_mode_changed()
{
	sp_log(Log::Debug, this) << "Updating cissearch... " << _settings->get(Set::Lib_SearchMode);

	m->lib_db->updateArtistCissearch();
	m->lib_db->updateAlbumCissearch();
	m->lib_db->updateTrackCissearch();

	sp_log(Log::Debug, this) << "Updating cissearch finished" << _settings->get(Set::Lib_SearchMode);
}


void LocalLibrary::library_reloading_state_new_block()
{
	m->reload_thread->pause();

    m->lib_db->getAllAlbums(_albums, _sortorder.so_albums);
    m->lib_db->getAllArtists(_artists, _sortorder.so_artists);
    m->lib_db->getAllTracks(_tracks, _sortorder.so_tracks);

	emit_stuff();

	m->reload_thread->goon();
}


void LocalLibrary::psl_disc_pressed(int disc)
{
	if( _selected_albums.size() != 1 )
	{
		return;
	}

	MetaDataList v_md;

    if(disc < 0)
    {
        m->lib_db->getAllTracksByAlbum(_selected_albums.first(), _tracks, _filter, _sortorder.so_tracks);
    }

    else
    {
        m->lib_db->getAllTracksByAlbum(_selected_albums.first(), v_md, _filter, _sortorder.so_tracks);

        _tracks.clear();

        for(const MetaData& md : v_md)
        {
            if(md.discnumber != disc) {
                continue;
            }

            _tracks << std::move(md);
        }
    }

    emit sig_all_tracks_loaded();
}




void LocalLibrary::get_all_artists(ArtistList& artists, Library::Sortings so)
{
	m->lib_db->getAllArtists(artists, so.so_artists);
}

void LocalLibrary::get_all_artists_by_searchstring(Library::Filter filter, ArtistList& artists, Library::Sortings so)
{
	m->lib_db->getAllArtistsBySearchString(filter, artists, so.so_artists);
}


void LocalLibrary::get_all_albums(AlbumList& albums, Library::Sortings so)
{
	m->lib_db->getAllAlbums(albums, so.so_albums);
}


void LocalLibrary::get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, Library::Filter filter, Library::Sortings so)
{
	m->lib_db->getAllAlbumsByArtist(artist_ids, albums, filter, so.so_albums)	;
}


void LocalLibrary::get_all_albums_by_searchstring(Library::Filter filter, AlbumList& albums, Library::Sortings so)
{
	m->lib_db->getAllAlbumsBySearchString(filter, albums, so.so_albums);
}


void LocalLibrary::get_all_tracks(MetaDataList& v_md, Library::Sortings so)
{
	m->lib_db->getAllTracks(v_md, so.so_tracks);
}


void LocalLibrary::get_all_tracks(const QStringList& paths, MetaDataList& v_md)
{
	m->lib_db->getMultipleTracksByPath(paths, v_md);
}


void LocalLibrary::get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, Library::Filter filter, Library::Sortings so)
{
	m->lib_db->getAllTracksByArtist(artist_ids, v_md, filter, so.so_tracks);
}


void LocalLibrary::get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, Library::Filter filter, Library::Sortings so)
{
	m->lib_db->getAllTracksByAlbum(album_ids, v_md, filter, so.so_tracks);
}


void LocalLibrary::get_all_tracks_by_searchstring(Library::Filter filter, MetaDataList& v_md, Library::Sortings so)
{
	m->lib_db->getAllTracksBySearchString(filter, v_md, so.so_tracks);
}


void LocalLibrary::get_album_by_id(int album_id, Album& album)
{
	m->lib_db->getAlbumByID(album_id, album);
}


void LocalLibrary::get_artist_by_id(int artist_id, Artist& artist)
{
	m->lib_db->getArtistByID(artist_id, artist);
}


void LocalLibrary::update_track(const MetaData& md)
{
	m->lib_db->updateTrack(md);
}

void LocalLibrary::update_tracks(const MetaDataList& v_md)
{
	m->lib_db->updateTracks(v_md);
}

void LocalLibrary::update_album(const Album& album)
{
	m->lib_db->updateAlbum(album);
}


void LocalLibrary::insert_tracks(const MetaDataList &v_md)
{
	m->lib_db->storeMetadata(v_md);
	AbstractLibrary::insert_tracks(v_md);
}


void LocalLibrary::init_reload_thread()
{
	if(m->reload_thread){
		return;
	}

	m->reload_thread = ReloadThread::getInstance();

	connect(m->reload_thread, &ReloadThread::sig_reloading_library,
			this, &LocalLibrary::sig_reloading_library);

	connect(m->reload_thread, &ReloadThread::sig_new_block_saved,
			this, &LocalLibrary::library_reloading_state_new_block);

	connect(m->reload_thread, &ReloadThread::finished,
			this, &LocalLibrary::reload_thread_finished);
}


void LocalLibrary::delete_tracks(const MetaDataList &v_md, Library::TrackDeletionMode mode)
{
	m->lib_db->deleteTracks(v_md);

	AbstractLibrary::delete_tracks(v_md, mode);
}

void LocalLibrary::refresh_artist() {}

void LocalLibrary::refresh_albums() {}

void LocalLibrary::refresh_tracks() {}

void LocalLibrary::import_files(const QStringList& files)
{
	if(!m->library_importer){
		m->library_importer = new LibraryImporter(this);
	}

	m->library_importer->import_files(files);
	emit sig_import_dialog_requested();
}


/** BIG TODO
 * What is the library for? Imo, the Library is there
 * for managing the database entries for the tracks
 * So, the library is NOT responsible for changing
 * The ID3 tags on filesystem base. So, these 3
 * methods should be moved somewhere else.
 * You can use the updateTracks method for doing
 * the database part when editing tracks.
 * But I suggest, to introduce a Library/TagEdit
 * interface which you can use to edit tracks. But
 * this is not part of this ticket.
 */

void LocalLibrary::merge_artists(const SP::Set<ArtistID>& artist_ids, ArtistID target_artist)
{
	if(artist_ids.isEmpty()) {
		return;
	}

	if(target_artist < 0){
		sp_log(Log::Warning, this) << "Cannot merge artist: Target artist id < 0";
		return;
	}

	bool show_album_artists = _settings->get(Set::Lib_ShowAlbumArtists);

	Artist artist;
	bool success = m->lib_db->getArtistByID(target_artist, artist);
	if(!success){
		return;
	}

	MetaDataList v_md;

	get_all_tracks_by_artist(artist_ids.toList(), v_md, _filter, _sortorder);
	tag_edit()->set_metadata(v_md);

	for(int idx=0; idx<v_md.count(); idx++)
	{
		MetaData md(v_md[idx]);
		if(show_album_artists){
			md.set_album_artist(artist.name, artist.id);
		}

		else {
			md.artist_id = artist.id;
			md.artist = artist.name;
		}

		tag_edit()->update_track(idx, md);
	}

	tag_edit()->commit();
}

void LocalLibrary::merge_albums(const SP::Set<AlbumID>& album_ids, AlbumID target_album)
{
	if(album_ids.isEmpty())	{
		return;
	}

	if(target_album < 0){
		sp_log(Log::Warning, this) << "Cannot merge albums: Target album id < 0";
		return;
	}

	Album album;
	bool success = m->lib_db->getAlbumByID(target_album, album, true);
	if(!success) {
		return;
	}

	MetaDataList v_md;
	get_all_tracks_by_album(album_ids.toList(), v_md, _filter, _sortorder);

	tag_edit()->set_metadata(v_md);

	for(int idx=0; idx<v_md.count(); idx++)
	{
		MetaData md(v_md[idx]);
		md.album_id = album.id;
		md.album = album.name;

		tag_edit()->update_track(idx, md);
	}

	tag_edit()->commit();
}

void LocalLibrary::change_track_rating(int idx, int rating)
{
    MetaDataList v_md{ _tracks[idx] };

	AbstractLibrary::change_track_rating(idx, rating);
    MetaData md_new = _tracks[idx];

	tag_edit()->set_metadata(v_md);
	tag_edit()->update_track(0, md_new);
	tag_edit()->commit();
}

void LocalLibrary::set_library_path(const QString& library_path)
{
	if(library_path == m->library_path){
		return;
	}

	LibraryManager* library_manager = LibraryManager::getInstance();
	library_manager->change_library_path(this->library_id(), library_path);

	m->library_path = library_path;

	emit sig_path_changed(library_path);
}

void LocalLibrary::set_library_name(const QString& library_name)
{
	if(library_name == m->library_name){
		return;
	}

	LibraryManager* library_manager = LibraryManager::getInstance();
	library_manager->rename_library(this->library_id(), library_name);

	m->library_name = library_name;

	emit sig_name_changed(library_name);
}

QString LocalLibrary::library_name() const
{
	return m->library_name;
}

QString LocalLibrary::library_path() const
{
	return m->library_path;
}

int8_t LocalLibrary::library_id() const
{
	return m->lib_id;
}

LibraryImporter* LocalLibrary::importer()
{
	if(!m->library_importer){
		m->library_importer = new LibraryImporter(this);
	}

	return m->library_importer;
}
