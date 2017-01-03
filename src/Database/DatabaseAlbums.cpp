/* DatabaseAlbums.cpp */

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
#include "Database/DatabaseAlbums.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Library/Filter.h"
#include "Helper/Library/DateFilter.h"

DatabaseAlbums::DatabaseAlbums(QSqlDatabase db, quint8 db_id) :
	DatabaseModule(db, db_id),
	DatabaseSearchMode(db)
{
	_artistid_field = "artistID";
	_fetch_query = "SELECT "
					"albums.albumID AS albumID"
					", albums.name AS albumName"
					", SUM(tracks.length) / 1000 AS albumLength"
					", albums.rating AS albumRating"
					", COUNT(DISTINCT tracks.trackid) AS trackCount"
					", MAX(tracks.year) AS albumYear"
					", GROUP_CONCAT(DISTINCT artists.name)"
					", GROUP_CONCAT(DISTINCT tracks.discnumber)"
					", GROUP_CONCAT(DISTINCT albumartists.name)"
					"FROM albums "
					"INNER JOIN tracks ON tracks.albumid = albums.albumid "
					"INNER JOIN artists ON tracks.artistid = artists.artistid "
					"LEFT OUTER JOIN artists albumartists ON tracks.albumartistid = albumartists.artistid "
	;
}


bool DatabaseAlbums::db_fetch_albums(SayonaraQuery& q, AlbumList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Could not get all albums from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element=q.first(); is_element; is_element = q.next())
	{
		Album album;

		album.id =			q.value(0).toInt();
		album.name =		q.value(1).toString().trimmed();
		album.length_sec =	q.value(2).toInt();
		album.rating =		q.value(3).toInt();
		album.num_songs =	q.value(4).toInt();
		album.year =		q.value(5).toInt();
		album.artists =		q.value(6).toString().split(',');

		album.discnumbers.clear();
		QStringList discs =	q.value(7).toString().split(',');
		discs.removeDuplicates();

		for(const QString& disc : discs) {
			album.discnumbers << disc.toInt();
		}

		if(album.discnumbers.isEmpty()) {
			album.discnumbers << 1;
		}

		album.set_album_artist(q.value(8).toString());

		album.n_discs = album.discnumbers.size();
		album.is_sampler = (album.artists.size() > 1);
		album.db_id = _module_db_id;

		result << album;
	};

	return true;
}

void DatabaseAlbums::set_album_fetch_query(const QString &query)
{
	_fetch_query = query;
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
	SayonaraQuery q (_db);
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


bool DatabaseAlbums::getAlbumByID(const int& id, Album& album)
{
	if(id == -1) {
		return false;
	}

	SayonaraQuery q(_db);
	QString querytext =	_fetch_query +
						" WHERE albums.albumID = :id "
						" GROUP BY albums.albumID, albums.name, albums.rating ";

	q.prepare(querytext);
	q.bindValue(":id", QVariant(id));

	AlbumList albums;
	db_fetch_albums(q, albums);

	if(albums.size() > 0) {
		album = albums.first();
	}

	return (albums.size() > 0);
}

bool DatabaseAlbums::getAllAlbums(AlbumList& result, Library::SortOrder sortorder)
{
	SayonaraQuery q (_db);
	QString querytext = _fetch_query ;

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
	SayonaraQuery q (_db);

	// fetch all albums
	QString querytext =	_fetch_query;

	if(artists.isEmpty()) {
		return false;
	}

	else if(artists.size() > 1) {
		querytext += "WHERE (artists.artistid = :artist_id ";
		for(int i=1; i<artists.size(); i++) {
			querytext += "OR artists.artistid = :artist_id_"
					+ QString::number(i) + " ";
		}

		querytext += ") ";
	}

	else{
		querytext += "WHERE artists.artistid = :artist_id ";
	}


	if( !filter.cleared() ) {

		switch(filter.mode())
		{
			case Library::Filter::Date:
				querytext += "AND " + filter.date_filter().get_sql_filter("tracks");
				break;

			case Library::Filter::Genre:
				querytext += "AND tracks.genre LIKE :filter1 ";			// track title is like filter
				break;

			case Library::Filter::Filename:
				querytext += "AND tracks.filename LIKE :filter1 ";			// track title is like filter

				break;

			case Library::Filter::Fulltext:
				querytext +=	"AND tracks.trackID IN ( "
								"	SELECT t2.trackID "
								"	FROM tracks t2 "
								"	WHERE t2.cissearch LIKE :filter1 "	// track title is like filter

								"	UNION SELECT t3.trackid "			// album title is like filter
								"	FROM tracks t3"
								"   INNER JOIN albums ON albums.albumid = t3.albumid "
								"	AND albums.cissearch LIKE :filter2 "

								"	UNION SELECT t4.trackid " 		// artist title is like filter
								"	FROM tracks t4 "
								"   INNER JOIN albums ON t4.albumid = albums.albumid "
								"   INNER JOIN artists ON t4." + _artistid_field + " = artists.artistid "
								"	AND artists.cissearch LIKE :filter3 "
								") ";
				break;

		}
	}

	querytext += QString("GROUP BY albums.albumID, albumName ");
	querytext += _create_order_string(sortorder) + ";";

	q.prepare(querytext);

	q.bindValue(":artist_id", QVariant(artists.first()));
	for(int i=1; i<artists.size(); i++) {
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}

	if( !filter.cleared() )
	{
		QString filtertext = filter.filtertext();
		switch(filter.mode())
		{
			case Library::Filter::Date:
				break;

			case Library::Filter::Fulltext:
				q.bindValue(":filter2", filtertext);
				q.bindValue(":filter3", filtertext);

			default:
				q.bindValue(":filter1", filtertext);
				break;
		}
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
	SayonaraQuery q(_db);
	QString querystring = _fetch_query;
	switch(filter.mode())
	{
		case Library::Filter::Date:
			querystring += "WHERE "
						 "(" + filter.date_filter().get_sql_filter("tracks") + ") "
						 "GROUP BY albums.albumID, albumName";
			break;
			
		case Library::Filter::Genre:
			querystring += "WHERE tracks.genre LIKE :search_in_genre "
						 "GROUP BY albums.albumID, albumName";
			break;

		case Library::Filter::Filename:
			querystring += "WHERE tracks.filename LIKE :search_in_filename "
						"GROUP BY albums.albumID, albumName";
			break;

		case Library::Filter::Fulltext:
		default:
			querystring = "SELECT DISTINCT * FROM ( "
						+ _fetch_query +
						"WHERE albums.cissearch LIKE :search_in_album "
						"GROUP BY albums.albumid, albums.name "
					"UNION "
						+ _fetch_query +
						"WHERE tracks.cissearch LIKE :search_in_title "
						"GROUP BY albums.albumid, albums.name "
					"UNION "
						+ _fetch_query +
						"WHERE artists.cissearch LIKE :search_in_artist "
						"GROUP BY albums.albumid, albums.name "
					""
				") "
				"GROUP BY albumID, albumName";
			break;
	}


	querystring += _create_order_string(sortorder) + ";";

	q.prepare(querystring);

	QString filtertext = filter.filtertext();
	switch(filter.mode())
	{
		case Library::Filter::Date:
			break;

		case Library::Filter::Genre:
			q.bindValue(":search_in_genre", filtertext);
			break;

		case Library::Filter::Filename:
			q.bindValue(":search_in_filename", filtertext);
			break;

		case Library::Filter::Fulltext:
		default:
			q.bindValue(":search_in_title", filtertext);
			q.bindValue(":search_in_album", filtertext);
			q.bindValue(":search_in_artist", filtertext);
			break;
	}

	return db_fetch_albums(q, result);
}


int DatabaseAlbums::updateAlbum (const Album & album)
{
	SayonaraQuery q(_db);

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
	getAllAlbums(albums);

	_db.transaction();

	for(const Album& album : albums)
	{
		QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
		SayonaraQuery q(_db);
		q.prepare(str);
		q.bindValue(":cissearch", Library::convert_search_string(album.name, search_mode()));
		q.bindValue(":id", album.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	_db.commit();
}

int DatabaseAlbums::insertAlbumIntoDatabase (const QString& album)
{
	SayonaraQuery q (_db);

	int album_id = getAlbumID(album);
	if(album_id >= 0){
		Album a;
		getAlbumByID(album_id, a);
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

	SayonaraQuery q (_db);
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
