/* DatabaseLibrary.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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

#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/Logger/Logger.h"

DatabaseLibrary::DatabaseLibrary(const QSqlDatabase& db, quint8 db_id) :
	DatabaseModule(db, db_id)
{
}


bool DatabaseLibrary::storeMetadata(const MetaDataList& v_md)  {

	DB_RETURN_NOT_OPEN_BOOL(_db);
	bool success = true;

	if(v_md.isEmpty()) {
		return success;
	}

	_db.transaction();

	DatabaseAlbums db_albums(_db, v_md.first().db_id);
	DatabaseArtists db_artists(_db, v_md.first().db_id);
	DatabaseTracks db_tracks(_db, v_md.first().db_id);

	AlbumList albums;
	ArtistList artists;
	QHash<QString, Album> album_map;
	QHash<QString, Artist> artist_map;

	db_albums.getAllAlbums(albums);
	db_artists.getAllArtists(artists);

	for(const Album& album : albums){
		album_map[album.name] = album;
	}

	for(const Artist& artist : artists){
		artist_map[artist.name] = artist;
	}

	albums.clear();
	artists.clear();

	for(const MetaData& md : v_md) {

		int artist_id, album_id, album_artist_id;
		//first check if we know the artist and its id
		Album album = album_map[md.album];
		if(album.id < 0) {
			album_id = db_albums.insertAlbumIntoDatabase(md.album);
			album.id = album_id;
			album_map[md.album] = album;
		}

		else{
			album_id = album.id;
		}

		Artist artist = artist_map[md.artist];
		if (artist.id < 0) {
			artist_id = db_artists.insertArtistIntoDatabase(md.artist);
			artist.id = artist_id;
			artist_map[md.artist] = artist;
		}

		else{
			artist_id = artist.id;
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

		if(album_id == -1 || artist_id == -1){
			sp_log(Log::Warning) << "Cannot insert artist or album of " << md.filepath();
			continue;
		}

		db_tracks.insertTrackIntoDatabase(md, artist_id, album_id, album_artist_id);
	}

	success = _db.commit();

	return success;
}

void DatabaseLibrary::createIndexes(){

	DB_RETURN_NOT_OPEN_VOID(_db);

	SayonaraQuery q(_db);
	QString querytext = QString() +
		"CREATE INDEX album_search ON albums(cissearch, albumID); "
		"CREATE INDEX artist_search ON artists(cissearch, artistID); "
		"CREATE INDEX track_search ON tracks(cissearch, trackID); ";

	q.prepare(querytext);

	q.exec();
}
