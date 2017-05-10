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
#include "Helper/Logger/Logger.h"

LibraryDatabase::LibraryDatabase(const QString& db_name, int db_id, qint8 library_id) :
	AbstractDatabase(db_id, "", db_name),
	DatabaseAlbums(db(), db_id, library_id),
	DatabaseArtists(db(), db_id, library_id),
	DatabaseTracks(db(), db_id, library_id),
	DatabaseLibrary(db(), db_id, library_id)
{
	_library_id = library_id;
	_db_id = db_id;
	db().open();

	bool show_album_artists = false;
	SayonaraQuery q(db());
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

	apply_fixes();
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

qint8 LibraryDatabase::library_id() const
{
	return _library_id;
}

int LibraryDatabase::db_id() const
{
	return _db_id;
}

bool LibraryDatabase::apply_fixes()
{
	if(_library_id < 0) { 
		return true;
	}

	QString track_view_name = "track_view_" + QString::number(_library_id);
	QString sql_string = "CREATE VIEW IF NOT EXISTS " + track_view_name +
        "  AS "
        "  SELECT "
        "  trackID "
        ", title "
        ", length "
        ", year "
        ", bitrate "
        ", filename "
        ", track "
        ", albumID "
        ", artistID "
        ", albumArtistID "
        ", genre "
        ", filesize "
        ", discnumber "
        ", rating "
        ", length "
        ", libraryID "
        "FROM tracks WHERE libraryID =" + QString::number(_library_id).left(2) + ";";

    if(!db().isOpen()){
        db().open();
    }

    SayonaraQuery q(db());
    q.prepare(sql_string);
    bool success = q.exec();
    if(!success) {
        q.show_error("Cannot create " + track_view_name);
    }

	return success;
}
