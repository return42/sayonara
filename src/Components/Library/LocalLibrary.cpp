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
#include "Importer/LibraryImporter.h"
#include "Threads/ReloadThread.h"
#include "Threads/IndexDirectoriesThread.h"
#include "Threads/FileSystemWatcher.h"
#include "Threads/UpdateDatesThread.h"
#include "Database/DatabaseConnector.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/TagEdit/TagEdit.h"

#include "Helper/Settings/Settings.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/Logger/Logger.h"
#include "Helper/globals.h"

#include <utility>

struct LocalLibrary::Private
{
	DatabaseConnector*	db=nullptr;
	ReloadThread* 		reload_thread=nullptr;
	FileSystemWatcher*	fsw=nullptr;
	TagEdit*			tag_edit=nullptr;

	Private()
	{
		db = DatabaseConnector::getInstance();
		tag_edit = new TagEdit(nullptr);
	}

	~Private()
	{
		delete tag_edit; tag_edit = nullptr;
	}
};

LocalLibrary::LocalLibrary(QObject *parent) :
	AbstractLibrary(parent)
{
	_m = Pimpl::make<Private>();

	apply_db_fixes();

	connect(_playlist, &PlaylistHandler::sig_track_deletion_requested, this, &LocalLibrary::delete_tracks);
	connect(_m->tag_edit, &TagEdit::finished, this, &LocalLibrary::refresh);

	REGISTER_LISTENER_NO_CALL(Set::Lib_SearchMode, _sl_search_mode_changed);
	REGISTER_LISTENER(Set::Lib_AutoUpdate, _sl_auto_update_changed);
}

LocalLibrary::~LocalLibrary() {}

void LocalLibrary::apply_db_fixes()
{
	QString str_val;
	_m->db->load_setting("version", str_val);

	int version = str_val.toInt();
	if(version < 11){
		UpdateDatesThread* t = new UpdateDatesThread(this);
		connect(t, &QThread::finished, t, &QObject::deleteLater);
		t->start();
	}
}


void LocalLibrary::psl_reload_library(bool clear_first, Library::ReloadQuality quality)
{
	if(_m->reload_thread && _m->reload_thread->is_running()){
		return;
	}

	if(!_m->reload_thread){
		init_reload_thread();
	}

	QString library_path = _settings->get(Set::Lib_Path);

	if(library_path.isEmpty()) {
		emit sig_no_library_path();
		return;
	}

	if(clear_first) {
		delete_all_tracks();
	}

	_m->reload_thread->set_quality(quality);
	_m->reload_thread->set_lib_path(library_path);
	_m->reload_thread->start();
}


void LocalLibrary::reload_thread_finished()
{
	load();

	emit sig_reloading_library("", -1);
	emit sig_reloading_library_finished();

	if(_m->fsw){
		_m->fsw->refresh();
	}
}

void LocalLibrary::_sl_search_mode_changed()
{
	sp_log(Log::Debug, this) << "Updating cissearch... " << _settings->get(Set::Lib_SearchMode);

	_m->db->updateArtistCissearch();
	_m->db->updateAlbumCissearch();
	_m->db->updateTrackCissearch();

	sp_log(Log::Debug, this) << "Updating cissearch finished" << _settings->get(Set::Lib_SearchMode);

}

void LocalLibrary::_sl_auto_update_changed()
{
	bool active = _settings->get(Set::Lib_AutoUpdate);
	if(active){
		MetaDataList v_md;
		get_all_tracks(v_md, Library::Sortings());
		IndexDirectoriesThread* thread = new IndexDirectoriesThread(v_md);
		connect(thread, &QThread::finished, this, &LocalLibrary::indexing_finished);
		thread->start();
	}

	else{
		if(_m->fsw){
			_m->fsw->stop();

			sp_log(Log::Debug, this) << "Removed filesystem watcher";
		}
	}
}

void LocalLibrary::indexing_finished()
{
	IndexDirectoriesThread* thread = dynamic_cast<IndexDirectoriesThread*>(sender());

	_m->fsw = new FileSystemWatcher(_settings->get(Set::Lib_Path), this);

	connect(_m->fsw, &QThread::finished, _m->fsw, &QObject::deleteLater);
	connect(_m->fsw, &QThread::destroyed, this, [=](){
		_m->fsw = nullptr;
	});

	connect(_m->fsw, &FileSystemWatcher::sig_changed, this, [=](){
		if(!_m->reload_thread || (_m->reload_thread && !_m->reload_thread->is_running())){
			this->psl_reload_library(false, Library::ReloadQuality::Fast);
		}
	});

	_m->fsw->start();

	thread->deleteLater();

	sp_log(Log::Debug, this) << "Added filesystem watcher";
}

void LocalLibrary::library_reloading_state_new_block()
{
	_m->reload_thread->pause();

	_m->db->getAllAlbums(_vec_albums, _sortorder.so_albums);
	_m->db->getAllArtists(_vec_artists, _sortorder.so_artists);
	_m->db->getAllTracks(_vec_md, _sortorder.so_tracks);

	emit_stuff();

	_m->reload_thread->goon();
}


void LocalLibrary::psl_disc_pressed(int disc)
{
	if( _selected_albums.size() != 1 )
	{
		return;
	}

	MetaDataList v_md;

	if(disc < 0){
		_m->db->getAllTracksByAlbum(_selected_albums.first(), _vec_md, _filter, _sortorder.so_tracks);
		emit sig_all_tracks_loaded(_vec_md);
		return;
	}

	_m->db->getAllTracksByAlbum(_selected_albums.first(), v_md, _filter, _sortorder.so_tracks);

	_vec_md.clear();

	for(const MetaData& md : v_md) {
		if(md.discnumber != disc) {
			continue;
		}

		_vec_md << std::move(md);
    }

	emit sig_all_tracks_loaded(_vec_md);
}




void LocalLibrary::get_all_artists(ArtistList& artists, Library::Sortings so)
{
	_m->db->getAllArtists(artists, so.so_artists);
}

void LocalLibrary::get_all_artists_by_searchstring(Library::Filter filter, ArtistList& artists, Library::Sortings so)
{
	_m->db->getAllArtistsBySearchString(filter, artists, so.so_artists);
}


void LocalLibrary::get_all_albums(AlbumList& albums, Library::Sortings so)
{
	_m->db->getAllAlbums(albums, so.so_albums);
}


void LocalLibrary::get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, Library::Filter filter, Library::Sortings so)
{
	_m->db->getAllAlbumsByArtist(artist_ids, albums, filter, so.so_albums)	;
}


void LocalLibrary::get_all_albums_by_searchstring(Library::Filter filter, AlbumList& albums, Library::Sortings so)
{
	_m->db->getAllAlbumsBySearchString(filter, albums, so.so_albums);
}


void LocalLibrary::get_all_tracks(MetaDataList& v_md, Library::Sortings so)
{
	_m->db->getAllTracks(v_md, so.so_tracks);
}


void LocalLibrary::get_all_tracks(const QStringList& paths, MetaDataList& v_md)
{
	_m->db->getMultipleTracksByPath(paths, v_md);
}


void LocalLibrary::get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, Library::Filter filter, Library::Sortings so)
{
	_m->db->getAllTracksByArtist(artist_ids, v_md, filter, so.so_tracks);
}


void LocalLibrary::get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, Library::Filter filter, Library::Sortings so)
{
	_m->db->getAllTracksByAlbum(album_ids, v_md, filter, so.so_tracks);
}


void LocalLibrary::get_all_tracks_by_searchstring(Library::Filter filter, MetaDataList& v_md, Library::Sortings so)
{
	_m->db->getAllTracksBySearchString(filter, v_md, so.so_tracks);
}


void LocalLibrary::get_album_by_id(int album_id, Album& album)
{
	_m->db->getAlbumByID(album_id, album);
}


void LocalLibrary::get_artist_by_id(int artist_id, Artist& artist)
{
	_m->db->getArtistByID(artist_id, artist);
}


void LocalLibrary::update_track(const MetaData& md)
{
	_m->db->updateTrack(md);
}

void LocalLibrary::update_tracks(const MetaDataList& v_md)
{
	_m->db->updateTracks(v_md);
}

void LocalLibrary::update_album(const Album& album)
{
	_m->db->updateAlbum(album);
}


void LocalLibrary::insert_tracks(const MetaDataList &v_md)
{
	_m->db->storeMetadata(v_md);
	AbstractLibrary::insert_tracks(v_md);
}


void LocalLibrary::init_reload_thread()
{
	if(_m->reload_thread){
		return;
	}

	_m->reload_thread = ReloadThread::getInstance();

	connect(_m->reload_thread, &ReloadThread::sig_reloading_library,
			this, &LocalLibrary::sig_reloading_library);

	connect(_m->reload_thread, &ReloadThread::sig_new_block_saved,
			this, &LocalLibrary::library_reloading_state_new_block);

	connect(_m->reload_thread, &ReloadThread::finished,
			this, &LocalLibrary::reload_thread_finished);
}


void LocalLibrary::delete_tracks(const MetaDataList &v_md, Library::TrackDeletionMode mode)
{
	_m->db->deleteTracks(v_md);

	AbstractLibrary::delete_tracks(v_md, mode);
}

void LocalLibrary::refresh_artist() {}

void LocalLibrary::refresh_albums() {}

void LocalLibrary::refresh_tracks() {}

void LocalLibrary::import_files(const QStringList& files)
{
	LibraryImporter::getInstance()->import_files(files);
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
	bool success = _m->db->getArtistByID(target_artist, artist);
	if(!success){
		return;
	}

	MetaDataList v_md;

	get_all_tracks_by_artist(artist_ids.toList(), v_md, _filter, _sortorder);
	_m->tag_edit->set_metadata(v_md);

	for(int idx=0; idx<v_md.size(); idx++)
	{
		MetaData md = v_md[idx];
		if(show_album_artists){
			md.set_album_artist(artist.name, artist.id);
		}

		else {
			md.artist_id = artist.id;
			md.artist = artist.name;
		}

		_m->tag_edit->update_track(idx, md);
	}

	_m->tag_edit->commit();
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
	bool success = _m->db->getAlbumByID(target_album, album, true);
	if(!success) {
		return;
	}

	MetaDataList v_md;
	get_all_tracks_by_album(album_ids.toList(), v_md, _filter, _sortorder);

	_m->tag_edit->set_metadata(v_md);
	for(int idx=0; idx<v_md.size(); idx++)
	{
		MetaData md = v_md[idx];
		md.album_id = album.id;
		md.album = album.name;

		_m->tag_edit->update_track(idx, md);
	}

	_m->tag_edit->commit();
}

void LocalLibrary::change_track_rating(int idx, int rating)
{
	MetaData md_old = _vec_md[idx];
	MetaDataList v_md; v_md << md_old;

	AbstractLibrary::change_track_rating(idx, rating);
	MetaData md_new = _vec_md[idx];

	_m->tag_edit->set_metadata(v_md);
	_m->tag_edit->update_track(0, md_new);
	_m->tag_edit->commit();
}
