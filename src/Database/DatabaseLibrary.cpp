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

	for(const MetaData& md : v_md) {

		int artist_id, album_id;
		//first check if we know the artist and its id

		album_id = db_albums.getAlbumID(md.album);
		if (album_id == -1) {
			album_id = db_albums.insertAlbumIntoDatabase(md.album);
		}

		artist_id = db_artists.getArtistID(md.artist);
		if (artist_id == -1) {
			artist_id = db_artists.insertArtistIntoDatabase(md.artist);
		}

		if(album_id == -1 || artist_id == -1){
			sp_log(Log::Warning) << "Cannot insert artist or album of " << md.filepath();
			continue;
		}

		db_tracks.insertTrackIntoDatabase (md, artist_id, album_id);
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
