/* DatabaseAlbums.cpp */

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
#include "Database/DatabaseAlbums.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Library/Filter.h"
#include "Helper/Library/DateFilter.h"

DatabaseAlbums::DatabaseAlbums(QSqlDatabase db, uint8_t db_id, int8_t library_id) :
	DatabaseSearchMode(db, db_id)
{
	_artistid_field = "artistID";

	if(library_id >= 0) {
		_track_view_name = QString("track_view_%1").arg(library_id);
		_search_view_name = QString("track_search_view_%1").arg(library_id);
	}

	else {
		_track_view_name = QString("tracks");
		_search_view_name = QString("track_search_view");
	}
}

DatabaseAlbums::~DatabaseAlbums() {}

QString DatabaseAlbums::fetch_query_albums(bool also_empty) const
{
	QString sql =
			"SELECT "
			"  albums.albumID AS albumID"
			", albums.name AS albumName"
			", SUM( " + _track_view_name + ".length) / 1000 AS albumLength"
			", albums.rating AS albumRating"
			", COUNT(DISTINCT " + _track_view_name + ".trackid) AS trackCount"
			", MAX(" + _track_view_name + ".year) AS albumYear"
			", GROUP_CONCAT(DISTINCT artists.name)"
			", GROUP_CONCAT(DISTINCT " + _track_view_name + ".discnumber)"
			", GROUP_CONCAT(DISTINCT albumArtists.name)"
			" FROM albums ";

	QString join = " INNER JOIN ";
	if(also_empty){
		join = " LEFT OUTER JOIN ";
	}

	sql +=	join + " " + _track_view_name + " ON " + _track_view_name + ".albumID = albums.albumID " +
			join + " artists ON ("
				   "    " + _track_view_name + ".artistID = artists.artistID OR "
				   "    " + _track_view_name + ".albumArtistID = artists.artistID"
				   " ) " +
			" LEFT OUTER JOIN artists albumArtists ON "
			"    " + _track_view_name + ".albumArtistID = albumArtists.artistID ";

	return sql;
}


bool DatabaseAlbums::db_fetch_albums(SayonaraQuery& q, AlbumList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all albums from database");
		return false;
	}

	result.reserve(q.fetched_rows());

	while(q.next())
	{
		Album album;

		album.id =			q.value(0).toInt();
		album.name =		q.value(1).toString().trimmed();
		album.length_sec =	q.value(2).toInt();
		album.rating =		q.value(3).toInt();
		album.num_songs =	q.value(4).toInt();
		album.year =		q.value(5).toInt();
		album.artists =		q.value(6).toString().split(',');
		album.set_album_artists(q.value(8).toString().split(','));

		album.discnumbers.clear();
		QStringList discs =	q.value(7).toString().split(',');
		discs.removeDuplicates();

		for(const QString& disc : discs) {
			album.discnumbers << disc.toInt();
		}

		if(album.discnumbers.isEmpty()) {
			album.discnumbers << 1;
		}

		album.n_discs = album.discnumbers.size();
		album.is_sampler = (album.artists.size() > 1);
		album.set_db_id(module_db_id());

		result.push_back(std::move(album));
	};

	return true;
}


QString DatabaseAlbums::_create_order_string(Library::SortOrder sortorder)
{
	switch(sortorder)
	{
		case Library::SortOrder::AlbumNameAsc:
			return QString (" ORDER BY albumName ASC ");

		case Library::SortOrder::AlbumNameDesc:
			return QString (" ORDER BY albumName DESC ");

		case Library::SortOrder::AlbumYearAsc:
			return QString (" ORDER BY albumYear ASC, albumName ASC ");

		case Library::SortOrder::AlbumYearDesc:
			return QString (" ORDER BY albumYear DESC, albumName ASC ");

		case Library::SortOrder::AlbumTracksAsc:
			return QString (" ORDER BY trackCount ASC, albumName ASC ");

		case Library::SortOrder::AlbumTracksDesc:
			return QString (" ORDER BY trackCount DESC, albumName ASC ");

		case Library::SortOrder::AlbumDurationAsc:
			return QString (" ORDER BY albumLength ASC, albumName ASC ");

		case Library::SortOrder::AlbumDurationDesc:
			return QString (" ORDER BY albumLength DESC, albumName ASC ");

		case Library::SortOrder::AlbumRatingAsc:
			return QString (" ORDER BY albumRating ASC, albumName ASC ");

		case Library::SortOrder::AlbumRatingDesc:
			return QString (" ORDER BY albumRating DESC, albumName ASC ");

		default:
			return "";
	}
}

int DatabaseAlbums::getAlbumID (const QString & album)
{
	SayonaraQuery q(this);
	int albumID = -1;

	q.prepare("SELECT albumID FROM albums WHERE name = ?;");
	q.addBindValue(album);

	if(!q.exec()) {
		return -1;
	}

	if (q.next()) {
		albumID = q.value(0).toInt();
	}

	return albumID;
}


bool DatabaseAlbums::getAlbumByID(const int& id, Album& album, bool also_empty)
{
	if(id == -1) {
		return false;
	}

	SayonaraQuery q(this);
	QString querytext =	fetch_query_albums(also_empty) +
						" WHERE albums.albumID = :id "
						" GROUP BY albums.albumID, albums.name, albums.rating ";

	q.prepare(querytext);
	q.bindValue(":id", id);

	AlbumList albums;
	db_fetch_albums(q, albums);

	if(albums.size() > 0) {
		album = albums.first();
	}

	return (albums.size() > 0);
}

bool DatabaseAlbums::getAllAlbums(AlbumList& result, bool also_empty)
{
	return getAllAlbums(result, Library::SortOrder::AlbumNameAsc, also_empty);
}

bool DatabaseAlbums::getAllAlbums(AlbumList& result, Library::SortOrder sortorder, bool also_empty)
{
	SayonaraQuery q(this);
	QString querytext = fetch_query_albums(also_empty);

	querytext += " GROUP BY albums.albumID, albums.name, albums.rating ";
	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	return db_fetch_albums(q, result);
}


bool DatabaseAlbums::getAllAlbumsByArtist(IDList artists, AlbumList& result)
{
	return getAllAlbumsByArtist(artists, result, Library::Filter());
}


bool DatabaseAlbums::getAllAlbumsByArtist(IDList artists, AlbumList& result, const Library::Filter& filter, Library::SortOrder sortorder)
{
	if(artists.isEmpty()) {
		return false;
	}

	SayonaraQuery q(this);
	QString select = "SELECT "
					 "  albumID"
					 ", albumName"
					 ", SUM(length) / 1000 AS albumLength"
					 ", albumRating"
					 ", COUNT(DISTINCT trackID) AS trackCount"
					 ", MAX(year) AS albumYear"
					 ", GROUP_CONCAT(DISTINCT artistName)"
					 ", GROUP_CONCAT(DISTINCT discnumber)"
					 ", GROUP_CONCAT(DISTINCT albumArtistName)"
					 " FROM " + _search_view_name + " ";
	QString query = select;

	if( !filter.cleared() )
	{
		switch(filter.mode())
		{
			case Library::Filter::Date:
				query = fetch_query_albums() + " WHERE " +
						filter.date_filter().get_sql_filter(_track_view_name) + " AND ";
				break;

			case Library::Filter::Genre:
				query += "WHERE genre LIKE :searchterm AND ";			// track title is like filter
				break;

			case Library::Filter::Filename:
				query += "WHERE filename LIKE :searchterm AND ";			// track title is like filter

				break;

			case Library::Filter::Fulltext:
				query += "WHERE allCissearch LIKE :searchterm AND ";
				break;
		}
	}

	else{
		query += "WHERE ";
	}

	if(!artists.isEmpty())
	{
		QString artist_id_field = _search_view_name + "." + _artistid_field;
		query += "(" + artist_id_field + " = :artist_id_0 ";

		for(int i=1; i<artists.size(); i++) {
			query += "OR " + artist_id_field + " = :artist_id_"
					+ QString::number(i) + " ";
		}

		query += ") ";
	}

	query += "GROUP BY albumID, albumName ";
	query += _create_order_string(sortorder) + ";";

	q.prepare(query);

	q.bindValue(":searchterm", filter.filtertext());

	for(int i=0; i<artists.size(); i++) {
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}

	return db_fetch_albums(q, result);
}

bool DatabaseAlbums::getAllAlbumsByArtist(int artist, AlbumList& result)
{
	return getAllAlbumsByArtist(artist, result, Library::Filter());
}

bool DatabaseAlbums::getAllAlbumsByArtist(int artist, AlbumList& result, const Library::Filter& filter, Library::SortOrder sortorder)
{
	IDList list;
	list << artist;
	return getAllAlbumsByArtist(list, result, filter, sortorder);
}


bool DatabaseAlbums::getAllAlbumsBySearchString(const Library::Filter& filter, AlbumList& result, Library::SortOrder sortorder)
{
	SayonaraQuery q(this);
	QString select = "SELECT "
					 "  albumID"
					 ", albumName"
					 ", SUM(length) / 1000 AS albumLength"
					 ", albumRating"
					 ", COUNT(DISTINCT trackID) AS trackCount"
					 ", MAX(year) AS albumYear"
					 ", GROUP_CONCAT(DISTINCT artistName)"
					 ", GROUP_CONCAT(DISTINCT discnumber)"
					 ", GROUP_CONCAT(DISTINCT albumArtistName)"
					 " FROM " + _search_view_name + " ";
	QString query;
	QString search_field;
	switch(filter.mode())
	{
		case Library::Filter::Date:
			query = fetch_query_albums() + " WHERE "
						 "(" + filter.date_filter().get_sql_filter(_track_view_name) + ") "
						 "GROUP BY albums.albumID, albumName";
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
					  "WHERE " + search_field + " LIKE :searchterm " +
					  "GROUP BY albumID, albumName ";
	}

	query += _create_order_string(sortorder) + ";";

	q.prepare(query);
	q.bindValue(":searchterm", filter.filtertext());

	return db_fetch_albums(q, result);
}

int DatabaseAlbums::updateAlbum (const Album & album)
{
	SayonaraQuery q(this);

	q.prepare("UPDATE albums "
			  "SET name=:name, "
			  "    cissearch=:cissearch, "
			  "    rating=:rating "
			  "WHERE albumID = :id;");

	QString cissearch = Library::convert_search_string(album.name, search_mode());

	q.bindValue(":id", album.id);
	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", cissearch);
	q.bindValue(":rating", album.rating);

	if (!q.exec()) {
		q.show_error(QString("Cannot update album ") + album.name);
		return -1;
	}

	return getAlbumID (album.name);
}

void DatabaseAlbums::updateAlbumCissearch()
{
	DatabaseSearchMode::update_search_mode();

	AlbumList albums;
	getAllAlbums(albums, true);

	module_db().transaction();

	for(const Album& album : albums)
	{
		QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
		SayonaraQuery q(this);
		QString cis = Library::convert_search_string(album.name, search_mode());
		q.prepare(str);
		q.bindValue(":cissearch", cis);
		q.bindValue(":id", album.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	module_db().commit();
}

int DatabaseAlbums::insertAlbumIntoDatabase (const QString& album)
{
	SayonaraQuery q(this);

	int album_id = getAlbumID(album);
	if(album_id >= 0){
		Album a;
		getAlbumByID(album_id, a, true);
		return updateAlbum(a);
	}

	QString cissearch = Library::convert_search_string(album, search_mode());
	q.prepare("INSERT INTO albums (name, cissearch) values (:album, :cissearch);");
	q.bindValue(":album", album);
	q.bindValue(":cissearch", cissearch);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert album ") + album + " to db");
		return -1;
	}

	return getAlbumID(album);
}

int DatabaseAlbums::insertAlbumIntoDatabase (const Album& album)
{
	if(album.id >= 0){
		return updateAlbum(album);
	}

	SayonaraQuery q(this);
	QString cissearch = Library::convert_search_string(album.name, search_mode());

	q.prepare("INSERT INTO albums (name, cissearch, rating) values (:name, :cissearch, :rating);");

	q.bindValue(":name", album.name);
	q.bindValue(":cissearch", cissearch);
	q.bindValue(":rating", album.rating);

	if (!q.exec()) {
		q.show_error("SQL: Cannot insert album into database");
		return -1;
	}

	return album.id;
}

void DatabaseAlbums::change_artistid_field(const QString& field)
{
	_artistid_field = field;
}

void DatabaseAlbums::change_track_lookup_field(const QString& track_lookup_field)
{
	_search_view_name = track_lookup_field;
}
