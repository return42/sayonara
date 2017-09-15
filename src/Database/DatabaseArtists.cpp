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

DatabaseArtists::DatabaseArtists(const QSqlDatabase& db, uint8_t db_id, int8_t library_id) :
	DatabaseSearchMode(db, db_id)
{
	_artistid_field = "artistID";
	_search_view_name = "track_search_view_" + QString::number(library_id);

	if(library_id < 0) {
		_track_view_name = QString("tracks");
	}

	else {
		_track_view_name = QString("track_view_%1").arg(library_id);
	}
}

QString DatabaseArtists::fetch_query_artists(bool also_empty) const
{
	QString sql =
			"SELECT "
			"artists.artistID AS artistID "
			", artists.name AS artistName "
			", COUNT(DISTINCT " + _track_view_name + ".trackID) AS trackCount "
			" FROM artists ";

	QString join = " INNER JOIN ";
	if(also_empty){
		join = " LEFT OUTER JOIN ";
	}


	sql += join + " " + _track_view_name + " ON " + _track_view_name + "." + _artistid_field + " = artists.artistID ";
	sql += join + " albums ON " + _track_view_name + ".albumID = albums.albumID ";

	return sql;
}

bool DatabaseArtists::db_fetch_artists(SayonaraQuery& q, ArtistList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all artists from database");
		return false;
	}

	result.reserve(q.fetched_rows());

	while(q.next())
	{
		Artist artist;

		artist.id = q.value(0).toInt();
		artist.name = q.value(1).toString().trimmed();
		artist.num_songs = q.value(2).toInt();
		artist.set_db_id(module_db_id());

		result << std::move(artist);
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

	SayonaraQuery q(this);

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
	SayonaraQuery q(this);
	QString query = "SELECT artistID FROM artists WHERE name = ?;";

	int artistID = -1;
	q.prepare(query);
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
	SayonaraQuery q(this);
	QString query = fetch_query_artists(also_empty);

	query += "GROUP BY artists.artistID, artists.name ";
	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	return db_fetch_artists(q, result);
}


bool DatabaseArtists::getAllArtistsBySearchString(const Library::Filter& filter, ArtistList& result, Library::SortOrder sortorder)
{
	SayonaraQuery q(this);
	QString query;
	QString select = "SELECT artistID, "
					 "artistName, "
					 "COUNT(DISTINCT trackID) AS trackCount "
					 "FROM " + _search_view_name + " ";

	QString search_field;

	switch(filter.mode())
	{
		case Library::Filter::Date:
			query = fetch_query_artists() +
					" AND (" + filter.date_filter().get_sql_filter(_track_view_name) + ") "
					" GROUP BY artists.artistID, artists.name ";
			break;

		case Library::Filter::Genre:
			search_field = "genre";
			break;

		case Library::Filter::Filename:
			search_field = "filename";
			break;

		case Library::Filter::Fulltext:
		default:
			search_field = "allCissearch";
			break;
	}

	if(query.isEmpty()){
			query = select +
					"WHERE " + search_field + " LIKE :searchterm "
					"GROUP BY artistID, artistName ";
	}

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);
	q.bindValue(":searchterm", filter.filtertext());

	return db_fetch_artists(q, result);
}


int DatabaseArtists::insertArtistIntoDatabase (const QString& artist)
{
	int id = getArtistID(artist);
	if(id >= 0){
		return id;
	}

	SayonaraQuery q(this);

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
	SayonaraQuery q(this);

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

	module_db().transaction();

	for(const Artist& artist : artists)
	{
		QString str = "UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";
		SayonaraQuery q(this);
		q.prepare(str);
		q.bindValue(":cissearch", Library::convert_search_string(artist.name, search_mode()));
		q.bindValue(":id", artist.id);

		if(!q.exec()){
			q.show_error("Cannot update artist cissearch");
		}
	}

	module_db().commit();
}


void DatabaseArtists::change_artistid_field(const QString& field)
{
	_artistid_field = field;
}

void DatabaseArtists::change_track_lookup_field(const QString& track_lookup_field)
{
	_search_view_name = track_lookup_field;
}
