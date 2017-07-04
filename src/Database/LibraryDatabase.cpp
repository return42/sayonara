/* LibraryDatabase.cpp */

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

#include "LibraryDatabase.h"
#include "SayonaraQuery.h"

LibraryDatabase::LibraryDatabase(uint8_t db_id, const QString& db_dir, const QString& db_name) :
	AbstractDatabase(db_id, db_dir, db_name),
	DatabaseAlbums(_database, db_id),
	DatabaseArtists(_database, db_id),
	DatabaseTracks(_database, db_id),
	DatabaseLibrary(_database, db_id)
{
	DB::getInstance()->add(this);

	bool show_album_artists = false;
	_database.open();
	SayonaraQuery q(_database);
	QString querytext = "SELECT value FROM settings WHERE key = 'lib_show_album_artists';";
	q.prepare(querytext);
	if(q.exec()){
		if(q.next()){
			QVariant var = q.value("value");
			show_album_artists = var.toBool();
		}
	}

	if(show_album_artists){
		change_artistid_field(LibraryDatabase::ArtistIDField::AlbumArtistID);
	}

	else{
		change_artistid_field(LibraryDatabase::ArtistIDField::ArtistID);
	}
}

LibraryDatabase::~LibraryDatabase() {}

void LibraryDatabase::change_artistid_field(LibraryDatabase::ArtistIDField field)
{
	QString str;
	if(field == LibraryDatabase::ArtistIDField::AlbumArtistID){
		str = "albumArtistID";
	}

	else{
		str = "artistID";
	}

	DatabaseAlbums::change_artistid_field(str);
	DatabaseArtists::change_artistid_field(str);
	DatabaseTracks::change_artistid_field(str);
}
