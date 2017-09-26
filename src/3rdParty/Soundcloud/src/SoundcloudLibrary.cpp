/* SoundcloudLibrary.cpp */

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

#include "SoundcloudLibrary.h"
#include "SoundcloudDataFetcher.h"
#include "SearchInformation.h"
#include "Sorting.h"

#include "Components/Covers/CoverLookup.h"
#include "Components/Covers/CoverLocation.h"

#include "Helper/Logger/Logger.h"
#include "Helper/Set.h"
#include "Helper/globals.h"

#include <QHash>

struct SC::Library::Private
{
    QHash<int, int>                 md_id_idx_map;
    QHash<int, SP::Set<int>>        md_artist_id_idx_map;
    QHash<int, SP::Set<int>>        md_album_id_idx_map;
    QHash<QString, SP::Set<int>>    md_name_idx_map;

    QHash<int, int>                 album_id_idx_map;
    QHash<QString, SP::Set<int>>    album_name_idx_map;
    QHash<QString, SP::Set<int>>    artist_name_album_idx_map;

    QHash<int, int>                 artist_id_idx_map;
    QHash<QString, SP::Set<int>>    artist_name_idx_map;

    MetaDataList    v_md;
    AlbumList       albums;
    ArtistList      artists;

    SC::Database*           scd=nullptr;
	SearchInformationList	search_information;

	Private()
	{
		scd = SC::Database::getInstance();
	}

	void clear_cache()
	{
		v_md.clear();
		albums.clear();
		artists.clear();
		search_information.clear();
	}
};

SC::Library::Library(QObject *parent) :
	AbstractLibrary(parent)
{
	_m = Pimpl::make<Private>();
}

SC::Library::~Library() {}

void SC::Library::load()
{
	AbstractLibrary::load();

	ArtistList artists;
	get_all_artists(artists);
}


void SC::Library::get_all_artists(ArtistList& artists, ::Library::Sortings so)
{
	if(_m->artists.isEmpty())
	{
		_m->scd->getAllArtists(artists, so.so_artists);
		_m->artists = artists;

        for(int i=0; i<_m->artists.size(); i++)
        {
			const Artist& artist = artists[i];
			_m->artist_id_idx_map[artist.id] = i;
			_m->artist_name_idx_map[artist.name].insert(i);
		}
	}

	else {
		artists = _m->artists;
	}

	SC::Sorting::sort_artists(artists, so.so_artists);
}

void SC::Library::get_all_artists_by_searchstring(::Library::Filter filter, ArtistList& artists, ::Library::Sortings so)
{
	if(filter.mode() != ::Library::Filter::Mode::Fulltext){
		return;
	}

	if(_m->search_information.is_empty()){
		_m->scd->getSearchInformation(_m->search_information);
	}

	SP::Set<int> artist_ids = _m->search_information.artist_ids(filter.filtertext());

    for(int artist_id : artist_ids)
    {
		int idx = _m->artist_id_idx_map[artist_id];
		artists << _m->artists[idx];
		artists.last().num_songs = _m->md_artist_id_idx_map[artist_id].size();
	}

	SC::Sorting::sort_artists(artists, so.so_artists);
}

void SC::Library::get_all_albums(AlbumList& albums, ::Library::Sortings so)
{
	if(_m->albums.isEmpty())
	{
		_m->scd->getAllAlbums(albums, false);
		_m->albums = albums;

		for(int i=0; i<albums.size(); i++)
		{
			const Album& album = albums[i];
			_m->album_id_idx_map[album.id] = i;
			_m->album_name_idx_map[album.name].insert(i);

			for(const QString& artist : album.artists){
				_m->artist_name_album_idx_map[artist].insert(i);
			}
		}
	}

	else {
		albums = _m->albums;
	}

	SC::Sorting::sort_albums(albums, so.so_albums);
}

void SC::Library::get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, ::Library::Filter filter, ::Library::Sortings so)
{
	Q_UNUSED(filter)

	for(int artist_id : artist_ids)
	{
		int artist_idx = _m->artist_id_idx_map[artist_id];
		const Artist& artist = _m->artists[artist_idx];

		SP::Set<int> album_idxs = _m->artist_name_album_idx_map[artist.name];

        for(int album_idx : album_idxs)
        {
            if(!between(album_idx, _m->albums)){
                sp_log(Log::Warning, this) << __FUNCTION__ << " Invalid index: " << album_idx << " (" << _m->albums.size() << ")";
            }
            else {
                albums.push_back(_m->albums[album_idx]);
            }
		}
	}

	SC::Sorting::sort_albums(albums, so.so_albums);
}

void SC::Library::get_all_albums_by_searchstring(::Library::Filter filter, AlbumList& albums, ::Library::Sortings so)
{
	if(filter.mode() != ::Library::Filter::Mode::Fulltext){
		return;
	}

	if(_m->search_information.is_empty()){
		_m->scd->getSearchInformation(_m->search_information);
	}

	SP::Set<int> album_ids = _m->search_information.album_ids(filter.filtertext());
    for(int album_id : album_ids)
    {
		int idx = _m->album_id_idx_map[album_id];
        if(!between(idx, _m->albums)) {
            sp_log(Log::Warning, this) << __FUNCTION__ << " Invalid index: " << idx << " (" << _m->albums.size() << ")";
        }

        else {
            albums << _m->albums[idx];
        }
	}

	SC::Sorting::sort_albums(albums, so.so_albums);
}

void SC::Library::get_all_tracks(const QStringList& paths, MetaDataList& v_md)
{
	Q_UNUSED(paths)
	Q_UNUSED(v_md)
	return;
}

void SC::Library::get_all_tracks(MetaDataList& v_md, ::Library::Sortings so)
{
	if(_m->v_md.isEmpty())
	{
		_m->scd->getAllTracks(v_md, so.so_tracks);
		_m->v_md = v_md;

        for(int i=0; i<_m->v_md.count(); i++)
		{
			const MetaData& md = v_md[i];

			_m->md_id_idx_map[md.id] = i;
			_m->md_name_idx_map[md.title].insert(i);
			_m->md_album_id_idx_map[md.album_id].insert(i);
			_m->md_artist_id_idx_map[md.artist_id].insert(i);
		}
	}

	else {
		v_md = _m->v_md;
	}

	SC::Sorting::sort_tracks(v_md, so.so_tracks);
}

#include <iostream>
void SC::Library::get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, ::Library::Filter filter, ::Library::Sortings so)
{
	Q_UNUSED(filter)

	for(int artist_id : artist_ids)
	{
		const SP::Set<int>& idxs = _m->md_artist_id_idx_map[artist_id];
        std::cout << "Get all tracks by artist " << artist_id << ": "
                                 << " Metadata has " << _m->v_md.size() << " elements" << std::endl;

        for(int idx : idxs)
        {
            if(!between(idx, _m->v_md)) {
                sp_log(Log::Warning, this) << __FUNCTION__ << " Invalid index: " << idx << " (" << _m->v_md.size() << ")";
            } else {
                v_md << _m->v_md[idx];
            }
		}
	}

	SC::Sorting::sort_tracks(v_md, so.so_tracks);
}

void SC::Library::get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, ::Library::Filter filter, ::Library::Sortings so)
{
	Q_UNUSED(filter)

	for(int album_id : album_ids)
	{
		const SP::Set<int>& idxs = _m->md_album_id_idx_map[album_id];
		for(int idx : idxs) {
			v_md << _m->v_md[idx];
		}
	}

	SC::Sorting::sort_tracks(v_md, so.so_tracks);
}

void SC::Library::get_all_tracks_by_searchstring(::Library::Filter filter, MetaDataList& v_md, ::Library::Sortings so)
{
	if(filter.mode() != ::Library::Filter::Mode::Fulltext){
		return;
	}

	if(_m->search_information.is_empty()){
		_m->scd->getSearchInformation(_m->search_information);
	}

	SP::Set<int> track_ids = _m->search_information.track_ids(filter.filtertext());

    for(int track_id : track_ids)
    {
		int idx = _m->md_id_idx_map[track_id];
		v_md << _m->v_md[idx];
	}

	SC::Sorting::sort_tracks(v_md, so.so_tracks);
}

void SC::Library::update_track(const MetaData& md)
{
	_m->scd->updateTrack(md);
    refetch();
}

void SC::Library::update_album(const Album& album)
{
	_m->scd->updateAlbum(album);
    refetch();
}

void SC::Library::delete_tracks(const MetaDataList& v_md, ::Library::TrackDeletionMode mode)
{
	Q_UNUSED(mode)

	_m->scd->deleteTracks(v_md);
    refetch();
}

void SC::Library::refetch()
{
    _m->clear_cache();

    AbstractLibrary::refetch();

    _m->scd->getSearchInformation(_m->search_information);
}

void SC::Library::psl_reload_library(bool b, ::Library::ReloadQuality quality)
{
	Q_UNUSED(b)
	Q_UNUSED(quality)

    _m->clear_cache();
}

void SC::Library::refresh_artist()
{
	if(_selected_artists.isEmpty()){
		return;
	}

	int artist_id = _selected_artists.first();

    MetaDataList v_md;
    get_all_tracks_by_artist({artist_id}, v_md, ::Library::Filter(), ::Library::Sortings());
    delete_tracks(v_md, ::Library::TrackDeletionMode::None);

	sp_log(Log::Debug, this) << "Deleted " << v_md.size() << " soundcloud tracks";

	SC::DataFetcher* fetcher = new SC::DataFetcher(this);

	connect(fetcher, &SC::DataFetcher::sig_artists_fetched,
			this, &SC::Library::artists_fetched);

	fetcher->get_artist(artist_id);
}


void SC::Library::refresh_albums() {}

void SC::Library::refresh_tracks() {}

void SC::Library::cover_found(const CoverLocation& cl)
{
	Q_UNUSED(cl)
	//sp_log(Log::Debug, this) << "Saved sound cloud cover: " << cl.toString();
}


void SC::Library::insert_tracks(const MetaDataList& v_md)
{
	Q_UNUSED(v_md)
	return;
}

void SC::Library::insert_tracks(const MetaDataList& v_md, const ArtistList& artists, const AlbumList& albums)
{
	Artist artist_tmp;
	Album album_tmp;

    for(const Artist& artist : artists)
    {
        if(!_m->scd->getArtistByID(artist.id, artist_tmp) || artist.id != artist_tmp.id) {
            _m->scd->insertArtistIntoDatabase(artist);
		}
	}

    for(const Album& album : albums)
    {
        if(!_m->scd->getAlbumByID(album.id, album_tmp) || album.id != album_tmp.id) {
            _m->scd->insertAlbumIntoDatabase(album);
		}

        else {
			album_tmp.print();
		}
	}

    if(!_m->scd->getAlbumByID(-1, album_tmp))
    {
		Album album;
		album.name = "None";
		album.id = 0;

		_m->scd->insertAlbumIntoDatabase(album);
	}

	_m->scd->storeMetadata(v_md);

	AbstractLibrary::insert_tracks(v_md);

    refetch();
}


void SC::Library::artists_fetched(const ArtistList& artists)
{
    for(const Artist& artist : artists)
    {
        sp_log(Log::Debug, this) << "Artist " << artist.name << " fetched";

		SC::DataFetcher* fetcher;

		if(artist.id <= 0) {
			continue;
		}

		_m->scd->updateArtist(artist);

		fetcher = new SC::DataFetcher(this);

		connect(fetcher, &SC::DataFetcher::sig_playlists_fetched,
				this, &SC::Library::albums_fetched);

		connect(fetcher, &SC::DataFetcher::sig_tracks_fetched,
				this, &SC::Library::tracks_fetched);

		fetcher->get_tracks_by_artist(artist.id);
	}

	sender()->deleteLater();
    refetch();
}

void SC::Library::tracks_fetched(const MetaDataList& v_md)
{
	for(const MetaData& md : v_md){
		if(md.id > 0){
			_m->scd->insertTrackIntoDatabase(md, md.artist_id, md.album_id);
		}
	}

	sender()->deleteLater();
    refetch();
}

void SC::Library::albums_fetched(const AlbumList& albums)
{
	for(const Album& album : albums){
		if(album.id > 0){
			_m->scd->insertAlbumIntoDatabase(album);
		}
	}

    sender()->deleteLater();
    refetch();
}


void SC::Library::get_artist_by_id(int artist_id, Artist& artist)
{
	Q_UNUSED(artist_id)
    Q_UNUSED(artist)
}


void SC::Library::get_album_by_id(int album_id, Album& album)
{
	Q_UNUSED(album_id)
	Q_UNUSED(album)
}
