/* DatabaseArtists.h */

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

#ifndef DATABASEARTISTS_H
#define DATABASEARTISTS_H

#include "Database/DatabaseSearchMode.h"
#include "Database/DatabaseModule.h"
#include "Helper/Library/Sortorder.h"

namespace Library {class Filter;}

class Artist;
class ArtistList;
class DatabaseArtists :
		private DatabaseModule,
		private DatabaseSearchMode
{

private:
	QString _artistid_field;
	QString _create_order_string(Library::SortOrder sort);

protected:
	void change_artistid_field(const QString& field);
	virtual QString fetch_query_artists() const;

public:

	DatabaseArtists(const QSqlDatabase& db, quint8 db_id);

	virtual bool db_fetch_artists(SayonaraQuery& q, ArtistList& result);

	virtual int getArtistID (const QString& artist);
	virtual bool getArtistByID(int id, Artist& artist);

	virtual bool getAllArtists(ArtistList& result, Library::SortOrder sortorder = Library::SortOrder::ArtistNameAsc);
	virtual bool getAllArtistsBySearchString(const Library::Filter& filter, ArtistList& result, Library::SortOrder sortorder = Library::SortOrder::ArtistNameAsc);

	virtual int insertArtistIntoDatabase(const QString& artist);
	virtual int insertArtistIntoDatabase(const Artist& artist);
	virtual int updateArtist(const Artist& artist);

	virtual void updateArtistCissearch();
};

#endif // DATABASEARTISTS_H
