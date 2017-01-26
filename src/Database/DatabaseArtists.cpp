/* DatabaseArtists.cpp */

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
#include "Database/DatabaseArtists.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/Library/Filter.h"
#include "Helper/Library/DateFilter.h"

DatabaseArtists::DatabaseArtists(const QSqlDatabase& db, quint8 db_id) :
	DatabaseModule(db, db_id),
	DatabaseSearchMode(db)
{
	_artistid_field = "artistID";
}

QString DatabaseArtists::fetch_query_artists(bool also_empty) const
{
	QString sql =
			"SELECT "
			"artists.artistID AS artistID "
			", artists.name AS artistName "
			", COUNT(DISTINCT tracks.trackid) AS trackCount "
			" FROM artists ";

	QString join = " INNER JOIN ";
	if(also_empty){
		join = " LEFT OUTER JOIN ";
	}

	sql += join + " tracks ON tracks." + _artistid_field + " = artists.artistID ";
	sql += join + " albums ON tracks.albumID = albums.albumID ";

	return sql;
}

bool DatabaseArtists::db_fetch_artists(SayonaraQuery& q, ArtistList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all artists from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element=q.first(); is_element; is_element = q.next()){
		Artist artist;

		artist.id = q.value(0).toInt();
		artist.name = q.value(1).toString().trimmed();
		artist.num_songs = q.value(2).toInt();
		artist.db_id = _module_db_id;

		result << artist;
	}

	return true;
}

QString DatabaseArtists::_create_order_string(Library::SortOrder sort)
{
	switch(sort) {
		case Library::SortOrder::ArtistNameAsc:
			return QString(" ORDER BY artistName ASC ");
		case Library::SortOrder::ArtistNameDesc:
			return QString(" ORDER BY artistName DESC ");
		case Library::SortOrder::ArtistTrackcountAsc:
			return QString(" ORDER BY trackCount ASC, artistName ASC ");
		case Library::SortOrder::ArtistTrackcountDesc:
			return QString(" ORDER BY trackCount DESC, artistName DESC ");
		default:
			return  "";
	}
}


bool DatabaseArtists::getArtistByID(int id, Artist& artist, bool also_empty)
{
	if(id < 0) return false;

	DB_RETURN_NOT_OPEN_BOOL(_db);

	SayonaraQuery q(_db);

	ArtistList artists;

	QString query = fetch_query_artists(also_empty) +
				"WHERE artists.artistID = ? "
				"GROUP BY artistName;";

	q.prepare(query);
	q.addBindValue(QVariant (id));

	bool success = db_fetch_artists(q, artists);

	if(artists.size() > 0) {
		success = true;
		artist = artists.first();
	}

	else success = false;

	return success;
}

int DatabaseArtists::getArtistID(const QString& artist)
{
	SayonaraQuery q(_db);
	int artistID = -1;
	q.prepare("SELECT artistID FROM artists WHERE name = ?;");
	q.addBindValue(artist);

	if (!q.exec()) {
		return -1;
	}

	if (q.next()) {
		artistID = q.value(0).toInt();
	}

	return artistID;
}

bool DatabaseArtists::getAllArtists(ArtistList& result, bool also_empty)
{
	return getAllArtists(result, Library::SortOrder::ArtistNameAsc, also_empty);
}

bool DatabaseArtists::getAllArtists(ArtistList& result, Library::SortOrder sortorder, bool also_empty)
{
	SayonaraQuery q(_db);
	QString query = fetch_query_artists(also_empty);

	query += "GROUP BY artists.artistID, artists.name ";
	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	return db_fetch_artists(q, result);
}

bool DatabaseArtists::getAllArtistsBySearchString(const Library::Filter& filter, ArtistList& result, Library::SortOrder sortorder)
{
	SayonaraQuery q(_db);
	QString query;

	switch(filter.mode())
	{
		case Library::Filter::Date:
			query = fetch_query_artists() +
					" AND (" + filter.date_filter().get_sql_filter("tracks") + ") "
					" GROUP BY artists.artistID, artists.name ";
			break;

		case Library::Filter::Genre:
			query = fetch_query_artists() +
					"   AND tracks.genre LIKE :searchstring "
					"	GROUP BY artists.artistID, artists.name ";
			break;

		case Library::Filter::Filename:
			query = fetch_query_artists() +
					"   AND tracks.filename LIKE :searchstring "
					"	GROUP BY artists.artistID, artists.name ";
			break;

		case Library::Filter::Fulltext:
		default:
			query = "SELECT * FROM ("
					+ fetch_query_artists() +
					" AND artists.cissearch LIKE :searchstring "
					" GROUP BY artists.artistID, artists.name "
					" UNION "
					+ fetch_query_artists() +
					" AND albums.cissearch LIKE :searchstring "
					" GROUP BY artists.artistID, artists.name "
					" UNION "
					+ fetch_query_artists() +
					" AND tracks.cissearch LIKE :searchstring "
					" GROUP BY artists.artistID, artists.name "
					") "
					"GROUP BY artistID, artistName ";
			break;
	}


	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	q.bindValue(":searchstring", filter.filtertext());
	return db_fetch_artists(q, result);
}

int DatabaseArtists::insertArtistIntoDatabase (const QString& artist)
{
	int id = getArtistID(artist);
	if(id >= 0){
		return id;
	}

	SayonaraQuery q(_db);

	QString cissearch = Library::convert_search_string(artist, search_mode());
	q.prepare("INSERT INTO artists (name, cissearch) values (:artist, :cissearch);");
	q.bindValue(":artist", artist);
	q.bindValue(":cissearch", cissearch);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert artist ") + artist);
		return -1;
	}

	return getArtistID(artist);
}

int DatabaseArtists::insertArtistIntoDatabase (const Artist & artist)
{
	if(artist.id >= 0){
		updateArtist(artist);
		return artist.id;
	}

	return insertArtistIntoDatabase(artist.name);
}


int DatabaseArtists::updateArtist(const Artist &artist)
{
	SayonaraQuery q(_db);

	if(artist.id < 0) return -1;

	QString cissearch = Library::convert_search_string(artist.name, search_mode());

	q.prepare("UPDATE artists SET name = :name, cissearch = :cissearch WHERE artistID = :artist_id;");
	q.bindValue(":name", artist.name);
	q.bindValue(":cissearch", cissearch);
	q.bindValue(":artist_id", artist.id);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert (2) artist ") + artist.name);
		return -1;
	}

	return artist.id;
}

void DatabaseArtists::updateArtistCissearch()
{
	DatabaseSearchMode::update_search_mode();

	ArtistList artists;
	getAllArtists(artists, true);

	_db.transaction();
	for(const Artist& artist : artists) {
		QString str = "UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";
		SayonaraQuery q(_db);
		q.prepare(str);
		q.bindValue(":cissearch", Library::convert_search_string(artist.name, search_mode()));
		q.bindValue(":id", artist.id);

		if(!q.exec()){
			q.show_error("Cannot update artist cissearch");
		}
	}
	_db.commit();
}


void DatabaseArtists::change_artistid_field(const QString& field)
{
	_artistid_field = field;
}
