/* DatabaseLibrary.cpp */

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

#include "Database/SayonaraQuery.h"
#include "Database/DatabaseLibrary.h"
#include "Database/DatabaseAlbums.h"
#include "Database/DatabaseArtists.h"
#include "Database/DatabaseTracks.h"

#include "Utils/MetaData/MetaData.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/Artist.h"
#include "Utils/Logger/Logger.h"

using DB::Library;
using DB::Query;

DB::Library::Library(const QSqlDatabase& db, uint8_t db_id, int8_t library_id) :
	Module(db, db_id)
{
	_library_id = library_id;
}

DB::Library::~Library() {}

bool DB::Library::storeMetadata(const MetaDataList& v_md)
{
	bool success = true;

	if(v_md.isEmpty()) {
		return success;
	}

	module_db().transaction();

	DB::Albums db_albums(module_db(), v_md.first().db_id(), _library_id);
	DB::Artists db_artists(module_db(), v_md.first().db_id(), _library_id);
	DB::Tracks db_tracks(module_db(), v_md.first().db_id(), _library_id);

	AlbumList albums;
	ArtistList artists;
	QHash<QString, Album> album_map;
	QHash<QString, Artist> artist_map;

	db_albums.getAllAlbums(albums, true);
	db_artists.getAllArtists(artists, true);

	for(const Album& album : albums){
		album_map[album.name()] = album;
	}

	for(const Artist& artist : artists){
		artist_map[artist.name()] = artist;
	}

	albums.clear();
	artists.clear();

	for(MetaData md : v_md)
	{
		int artist_id, album_id, album_artist_id;
		//first check if we know the artist and its id
		Album album = album_map[md.album()];
		if(album.id < 0) {
			album_id = db_albums.insertAlbumIntoDatabase(md.album());
			album.id = album_id;
			album_map[md.album()] = album;
		}

		else{
			album_id = album.id;
		}

		Artist artist = artist_map[md.artist()];
		if (artist.id < 0) {
			artist_id = db_artists.insertArtistIntoDatabase(md.artist());
			artist.id = artist_id;
			artist_map[md.artist()] = artist;
		}

		else{
			artist_id = artist.id;
		}

		if(md.album_artist_id() == -1){
			md.set_album_artist_id(artist_id);
		}

		if(md.album_artist().isEmpty()){
			md.set_album_artist(md.artist());
		}

		Artist album_artist = artist_map[md.album_artist()];
		if (album_artist.id < 0) {
			if(md.album_artist().isEmpty()){
				album_artist_id = -1;
			}

			else{
				album_artist_id = db_artists.insertArtistIntoDatabase(md.album_artist());
				album_artist.id = album_artist_id;
				artist_map[md.album_artist()] = album_artist;
			}
		}

		else{
			album_artist_id = album_artist.id;
		}

		md.album_id = album_id;
		md.artist_id = artist_id;
		md.library_id = _library_id;

		if(album_id == -1 || artist_id == -1 || md.library_id == -1){
			sp_log(Log::Warning) << "Cannot insert artist or album of " << md.filepath();
			continue;
		}

		db_tracks.insertTrackIntoDatabase(md, artist_id, album_id, album_artist_id);
	}

	success = module_db().commit();

	return success;
}

void DB::Library::addAlbumArtists()
{
	Query q(this);
	QString	querytext = "UPDATE tracks SET albumArtistID = artistID WHERE albumArtistID = -1;";
	q.prepare(querytext);
	if(!q.exec()){
		q.show_error("Cannot add album artists");
	}
}

void DB::Library::dropIndexes()
{
	QStringList indexes;
	indexes << "album_search";
	indexes << "artist_search";
	indexes << "track_search";

	for(const QString& idx : indexes)
	{
		Query q(this);
		QString text = "DROP INDEX " + idx + ";";
		q.prepare(text);
		if(!q.exec()){
			q.show_error("Cannot drop index " + idx);
		}
	}
}

using IndexDescription=std::tuple<QString, QString, QString>;
void DB::Library::createIndexes()
{
	dropIndexes();

	QList<IndexDescription> indexes;
	indexes << std::make_tuple("album_search", "albums", "albumID");
	indexes << std::make_tuple("artist_search", "artists", "artistID");
	indexes << std::make_tuple("track_search", "tracks", "trackID");

	for(const IndexDescription& idx : indexes)
	{
		Query q(this);
		QString name = std::get<0>(idx);
		QString table = std::get<1>(idx);
		QString column = std::get<2>(idx);
		QString text = "CREATE INDEX " + name + " ON " + table + " (cissearch, " + column + ");";
		q.prepare(text);
		if(!q.exec()){
			q.show_error("Cannot create index " + name);
		}
	}
}
