/* DatabaseTracks.cpp */

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
#include "Database/DatabaseTracks.h"
#include "Database/DatabaseLibrary.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Library/Filter.h"
#include "Helper/Library/DateFilter.h"

#include <QFileInfo>
#include <QDateTime>
#include <QMap>

#include <utility>
#include <tuple>

static MetaDataList all_tracks;

DatabaseTracks::DatabaseTracks(const QSqlDatabase& db, quint8 db_id, qint8 library_id) :
	DatabaseModule(db, db_id),
	DatabaseSearchMode(db)
{
	_artistid_field = "artistID";
	_library_id = library_id;
	_track_view_name = QString("track_view_") + QString::number(library_id);

	if(library_id < 0) {
		_track_view_name = QString("tracks");
	} 
}

QString DatabaseTracks::fetch_query_tracks() const
{
	return "SELECT "
		   "  " + _track_view_name + ".trackID AS trackID "			// 0
		   ", " + _track_view_name + ".title AS trackTitle "			// 1
		   ", " + _track_view_name + ".length AS trackLength "		// 2
		   ", " + _track_view_name + ".year AS trackYear "			// 3
		   ", " + _track_view_name + ".bitrate AS trackBitrate "		// 4
		   ", " + _track_view_name + ".filename AS trackFilename "	// 5
		   ", " + _track_view_name + ".track AS trackNum "			// 6
		   ", albums.albumID AS albumID "			// 7
		   ", artists.artistID AS artistID "		// 8
		   ", albums.name AS albumName "			// 9
		   ", artists.name AS artistName "			// 10
		   ", " + _track_view_name + ".genre AS genrename "			// 11
		   ", " + _track_view_name + ".filesize AS filesize "			// 12
		   ", " + _track_view_name + ".discnumber AS discnumber "		// 13
		   ", " + _track_view_name + ".rating AS rating "				//14
		   ", " + _track_view_name + ".albumArtistID "				// 15
		   ", albumartists.name "					// 16
		   ", " + _track_view_name + ".libraryID " //17
		   "FROM " + _track_view_name + " "
		   "INNER JOIN albums ON " + _track_view_name + ".albumID = albums.albumID "
		   "INNER JOIN artists ON " + _track_view_name + ".artistID = artists.artistID "
		   "LEFT OUTER JOIN artists albumartists ON " + _track_view_name + ".albumArtistID = albumartists.artistID "
		   //"WHERE filetype is null "
			;
}


bool DatabaseTracks::db_fetch_tracks(SayonaraQuery& q, MetaDataList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Cannot fetch tracks from database");
		return false;
	}

	if(!q.last()){
		return true;
	}

	for(bool is_element = q.first(); is_element; is_element = q.next()){
		MetaData data;

		data.id = 		 q.value(0).toInt();
		data.title = 	 q.value(1).toString();
		data.length_ms = q.value(2).toInt();
		data.year = 	 q.value(3).toInt();
		data.bitrate = 	 q.value(4).toInt();
		data.set_filepath(q.value(5).toString());
		data.track_num = q.value(6).toInt();
		data.album_id =  q.value(7).toInt();
		data.artist_id = q.value(8).toInt();
		data.album = 	 q.value(9).toString().trimmed();
		data.artist = 	 q.value(10).toString().trimmed();
		data.set_genres(q.value(11).toString().split(","));
		data.filesize =  q.value(12).toInt();
		data.discnumber = q.value(13).toInt();
		data.rating = q.value(14).toInt();
		data.set_album_artist(q.value(16).toString(), q.value(15).toInt());
		data.library_id = q.value(17).toInt();
		data.db_id = _module_db_id;

		result << data;
	}

	return true;
}


QString DatabaseTracks::append_track_sort_string(QString querytext, Library::SortOrder sort)
{
	if(sort == Library::SortOrder::TrackArtistAsc) querytext += QString(" ORDER BY artistName ASC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackArtistDesc) querytext += QString(" ORDER BY artistName DESC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackAlbumAsc) querytext += QString(" ORDER BY discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackAlbumDesc) querytext += QString(" ORDER BY discnumber ASC, albumName DESC, trackNum;");
	else if(sort == Library::SortOrder::TrackTitleAsc) querytext += QString(" ORDER BY trackTitle ASC;");
	else if(sort == Library::SortOrder::TrackTitleDesc) querytext += QString(" ORDER BY trackTitle DESC;");
	else if(sort == Library::SortOrder::TrackNumAsc) querytext += QString(" ORDER BY trackNum ASC;");
	else if(sort == Library::SortOrder::TrackNumDesc) querytext += QString(" ORDER BY trackNum DESC;");
	else if(sort == Library::SortOrder::TrackYearAsc) querytext += QString(" ORDER BY trackYear ASC;");
	else if(sort == Library::SortOrder::TrackYearDesc) querytext += QString(" ORDER BY trackYear DESC;");
	else if(sort == Library::SortOrder::TrackLenghtAsc) querytext += QString(" ORDER BY trackLength ASC;");
	else if(sort == Library::SortOrder::TrackLengthDesc) querytext += QString(" ORDER BY trackLength DESC;");
	else if(sort == Library::SortOrder::TrackBitrateAsc) querytext += QString(" ORDER BY trackBitrate ASC;");
	else if(sort == Library::SortOrder::TrackBitrateDesc) querytext += QString(" ORDER BY trackBitrate DESC;");
	else if(sort == Library::SortOrder::TrackSizeAsc) querytext += QString(" ORDER BY filesize ASC;");
	else if(sort == Library::SortOrder::TrackSizeDesc) querytext += QString(" ORDER BY filesize DESC;");
	else if(sort == Library::SortOrder::TrackRatingAsc) querytext += QString(" ORDER BY rating ASC;");
	else if(sort == Library::SortOrder::TrackRatingDesc) querytext += QString(" ORDER BY rating DESC;");

	else querytext += ";";

	return querytext;
}


bool DatabaseTracks::getMultipleTracksByPath(const QStringList& paths, MetaDataList& v_md)
{
	_db.transaction();

	for(const QString& path : paths) {
		v_md << getTrackByPath(path);
	}

	_db.commit();

	return (v_md.size() == paths.size());
}


MetaData DatabaseTracks::getTrackByPath(const QString& path)
{
	SayonaraQuery q(_db);

	QString querytext = fetch_query_tracks() + " WHERE " + _track_view_name + ".filename LIKE :filename;";
	sp_log(Log::Debug, this) << __FUNCTION__ << fetch_query_tracks();
	q.prepare(querytext);
	q.bindValue(":filename", path);

	MetaData md(path);
	md.db_id = _module_db_id;

	MetaDataList v_md;
	if(!db_fetch_tracks(q, v_md)) {
		return md;
	}

	if(v_md.size() == 0) {
		md.is_extern = true;

		return md;
	}

	return v_md.first();
}


MetaData DatabaseTracks::getTrackById(int id)
{
	SayonaraQuery q(_db);
	QString querytext = fetch_query_tracks() + " WHERE " + _track_view_name + ".trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", QVariant(id));

	MetaDataList v_md;
	if(!db_fetch_tracks(q, v_md)) {
		return MetaData();
	}

	if(v_md.isEmpty()) {
		MetaData md;
		md.is_extern = true;
		return md;
	}

	return v_md.first();
}


bool DatabaseTracks::getAllTracks(MetaDataList& returndata, Library::SortOrder sort)
{
	SayonaraQuery q(_db);

	QString querytext = append_track_sort_string(fetch_query_tracks(), sort);

	q.prepare(querytext);

	return db_fetch_tracks(q, returndata);
}


bool DatabaseTracks::getAllTracksByAlbum(int album, MetaDataList& result)
{
	return getAllTracksByAlbum(album, result, Library::Filter());
}


bool DatabaseTracks::getAllTracksByAlbum(int album, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort, int discnumber)
{
	bool success;
	IDList list;
	MetaDataList v_md;

	list << album;
	returndata.clear();

	success = getAllTracksByAlbum(list, v_md, filter, sort);

	if(discnumber < 0) {
		returndata = v_md;
	}

	for(const MetaData& md : v_md) {
		if(discnumber != md.discnumber) {
			continue;
		}

		returndata << std::move(md);
	}

	return success;
}


bool DatabaseTracks::getAllTracksByAlbum(IDList albums, MetaDataList& result)
{
	return getAllTracksByAlbum(albums, result, Library::Filter());
}


bool DatabaseTracks::getAllTracksByAlbum(IDList albums, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort)
{
	SayonaraQuery q(_db);
	QString querytext = fetch_query_tracks();

	if(albums.isEmpty()) {
		return false;
	}

	if( !filter.cleared() )
	{
		switch(filter.mode())
		{
			case Library::Filter::Date:
				querytext += "WHERE " + filter.date_filter().get_sql_filter(_track_view_name) + " AND ";
				break;

			case Library::Filter::Genre:
				querytext += "WHERE " + _track_view_name + ".genre LIKE :filter1 AND ";
				break;

			case Library::Filter::Filename:
				querytext += "WHERE " + _track_view_name + ".filename LIKE :filter1 AND ";
				break;

			case Library::Filter::Fulltext:
				// consider the case, that the search string may fit to the title
				// union the case that the search string may fit to the album
				querytext +=
								" INNER JOIN ( "
								"	SELECT " + _track_view_name + ".trackID "
								"	FROM " + _track_view_name + " "
								"	WHERE " + _track_view_name + ".cissearch LIKE :filter1 "

								"	UNION SELECT " + _track_view_name + ".trackID "
								"	FROM " + _track_view_name + " "
								"   INNER JOIN albums "
								"   ON " + _track_view_name + ".albumID = albums.albumID "
								"	WHERE albums.cissearch LIKE :filter2 "

								"	UNION SELECT " + _track_view_name + ".trackID "
								"	FROM " + _track_view_name + " "
								"   INNER JOIN albums "
								"   ON " + _track_view_name + ".albumID = albums.albumID "
								"   INNER JOIN artists "
								"   ON " + _track_view_name + ".artistID = artists.artistID "
								"   WHERE artists.cissearch LIKE :filter3 "

								"	UNION SELECT " + _track_view_name + ".trackID "
								"	FROM " + _track_view_name + " "
								"   INNER JOIN albums "
								"   ON " + _track_view_name + ".albumID = albums.albumID "
								"   INNER JOIN artists "
								"   ON " + _track_view_name + ".albumArtistID = artists.artistID "
								"   WHERE artists.cissearch LIKE :filter4 "
								") foundTracks ON " + _track_view_name + ".trackID = foundTracks.trackID WHERE "
						;
				break;

			default:
				break;
		}
	}

	else{
		querytext += " WHERE ";
	}

	if(albums.size() == 1) {
		querytext += " " + _track_view_name + ".albumID=:albumid ";
	}

	else {
		querytext += " (" + _track_view_name + ".albumID=:albumid ";
		for(int i=1; i<albums.size(); i++) {
			querytext += "OR " + _track_view_name + ".albumID=:albumid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}


	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":albumid", QVariant(albums.first()));
	for(int i=1; i<albums.size(); i++) {
		q.bindValue(QString(":albumid_") + QString::number(i), albums[i]);
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
				q.bindValue(":filter4", filtertext);

			default:
				q.bindValue(":filter1", filtertext);
				break;
		}
	}

	return db_fetch_tracks(q, returndata);
}

bool DatabaseTracks::getAllTracksByArtist(int artist, MetaDataList& result)
{
	return getAllTracksByArtist(artist, result, Library::Filter());
}

bool DatabaseTracks::getAllTracksByArtist(int artist, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort)
{
	IDList list;
	list << artist;
	return getAllTracksByArtist(list, returndata, filter, sort);
}

bool DatabaseTracks::getAllTracksByArtist(IDList artists, MetaDataList& returndata)
{
	return 	getAllTracksByArtist(artists, returndata, Library::Filter());
}

bool DatabaseTracks::getAllTracksByArtist(IDList artists, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort)
{
	SayonaraQuery q(_db);
	QString querytext = fetch_query_tracks();

	if(artists.size() == 0){
		return false;
	}

	if( !filter.cleared() )
	{
		switch( filter.mode() )
		{
			case Library::Filter::Date:
				querytext += "WHERE " + filter.date_filter().get_sql_filter(_track_view_name) + " AND ";
				break;

			case Library::Filter::Genre:
				querytext += "WHERE " + _track_view_name + ".genre LIKE :filter1 AND " ;
				break;

			case Library::Filter::Filename:
				querytext += "AND " + _track_view_name + ".filename LIKE :filter1 AND ";
				break;

			case Library::Filter::Fulltext:
			default:
				querytext += "INNER JOIN ( "
							 "	SELECT " + _track_view_name + ".trackid "
							 "	FROM " + _track_view_name + " "
							 "	WHERE " + _track_view_name + ".cissearch LIKE :filter1 "

							 "	UNION SELECT " + _track_view_name + ".trackID "
							 "	FROM " + _track_view_name + " "
							 "	INNER JOIN albums ON "
							 "	albums.albumID = " + _track_view_name + ".albumID "
							 "  AND albums.cissearch LIKE :filter2 "

							 "	UNION SELECT " + _track_view_name + ".trackID "
							 "	FROM " + _track_view_name + " "
							 "  INNER JOIN artists ON "
							 "  " + _track_view_name + ".artistID = artists.artistID "
							 "  AND artists.cissearch LIKE :filter3"

							 "	UNION SELECT " + _track_view_name + ".trackID "
							 "	FROM " + _track_view_name + " "
							 "  INNER JOIN artists albumArtists "
							 "  ON " + _track_view_name + ".albumArtistID = albumArtists.artistID "
							 "  AND albumArtists.cissearch LIKE :filter4 "
							 ") foundTracks ON " + _track_view_name + ".trackID = foundTracks.trackID WHERE ";
				break;
		}
	}

	else{
		querytext += " WHERE ";
	}

	if(artists.size() == 1) {
		querytext += " " + _track_view_name + "." + _artistid_field + "=:artist_id ";
	}

	else {
		querytext += " (" + _track_view_name + "." + _artistid_field + "=:artist_id ";
		for(int i=1; i<artists.size(); i++) {
			querytext += "OR " + _track_view_name + "." + _artistid_field + "=:artist_id_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	querytext = append_track_sort_string(querytext, sort);

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
				q.bindValue(":filter4", filtertext);

			default:
				q.bindValue(":filter1", filtertext);
				break;
		}
	}

	return db_fetch_tracks(q, returndata);
}


bool DatabaseTracks::getAllTracksBySearchString(const Library::Filter& filter, MetaDataList& result, Library::SortOrder sort)
{
	SayonaraQuery q(_db);
	QString querytext;

	switch(filter.mode())
	{
		case Library::Filter::Date:
			querytext = fetch_query_tracks() + " WHERE " + filter.date_filter().get_sql_filter(_track_view_name);
			break;

		case Library::Filter::Genre:
			querytext = fetch_query_tracks() +
					"WHERE genrename LIKE :search_in_genre ";
			break;

		case Library::Filter::Filename:
			querytext = fetch_query_tracks() +
					"WHERE " + _track_view_name + ".filename LIKE :search_in_filename ";
			break;

		case Library::Filter::Fulltext:
			querytext = fetch_query_tracks() +
					"INNER JOIN ("
					"SELECT " + _track_view_name + ".trackID "
					"FROM " + _track_view_name + " "
					"WHERE " + _track_view_name + ".cissearch LIKE :search_in_title "

					"UNION "
					"SELECT " + _track_view_name + ".trackID "
					"FROM " + _track_view_name + " "
					"INNER JOIN albums ON " + _track_view_name + ".albumID = albums.albumID "
					"AND albums.cissearch LIKE :search_in_album "

					"UNION "
					"SELECT " + _track_view_name + ".trackID "
					"FROM " + _track_view_name + " "
					"INNER JOIN artists ON "
					"" + _track_view_name + ".artistID = artists.artistID "
					"AND artists.cissearch LIKE :search_in_artist "

					"UNION "
					"SELECT " + _track_view_name + ".trackID "
					"FROM " + _track_view_name + " "
					"INNER JOIN artists ON "
					"" + _track_view_name + ".albumArtistID = artists.artistID "
					"AND artists.cissearch LIKE :search_in_artist "

					") foundTracks ON " + _track_view_name + ".trackID = foundTracks.trackID";

			break;
	}

	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);

	QString filtertext = filter.filtertext();
	switch(filter.mode())
	{
		case Library::Filter::Genre:
			q.bindValue(":search_in_genre", filtertext);
			break;

		case Library::Filter::Filename:
			q.bindValue(":search_in_filename", filtertext);
			break;

		case Library::Filter::Fulltext:
			q.bindValue(":search_in_title", filtertext);
			q.bindValue(":search_in_album", filtertext);
			q.bindValue(":search_in_artist", filtertext);
			break;

		default:
			break;
	}

	return db_fetch_tracks(q, result);
}

bool DatabaseTracks::deleteTrack(int id)
{
	SayonaraQuery q(_db);
	QString querytext = QString("DELETE FROM tracks WHERE trackID = :track_id;");

	q.prepare(querytext);
	q.bindValue(":track_id", id);

	if (!q.exec()) {
		q.show_error(QString("Cannot delete track") + QString::number(id));
		return false;
	}

	return true;
}


bool DatabaseTracks::deleteTracks(const IDList& ids)
{
	int n_files = 0;
	bool success;

	_db.transaction();

	for(const int& id : ids){
		if( deleteTrack(id) ){
			n_files++;
		};
	}

	success = _db.commit();

	return success && (n_files == ids.size());
}


bool DatabaseTracks::deleteTracks(const MetaDataList& v_md)
{
	int success = 0;

	_db.transaction();

	for(const MetaData& md : v_md){
		if( deleteTrack(md.id) ){
			success++;
		};
	}

	_db.commit();

	sp_log(Log::Info) << "Deleted " << success << " of " << v_md.size() << " tracks";

	return (success == v_md.size());
}

bool DatabaseTracks::deleteInvalidTracks()
{
	bool success;

	MetaDataList v_md;
	QMap<QString, int> map;
	IDList to_delete;
	MetaDataList v_md_update;

	SayonaraQuery q(_db);
	DatabaseLibrary db_library(_db, _module_db_id, _library_id);

	if(!getAllTracks(v_md)){
		sp_log(Log::Error) << "Cannot get tracks from db";
		return false;
	}

	int idx = 0;
	for(const MetaData& md : v_md)
	{
		if(map.contains(md.filepath())){
			sp_log(Log::Warning) << "found double path: " << md.filepath();
			int old_idx = map[md.filepath()];
			to_delete << md.id;
			v_md_update << v_md[old_idx];
		}

		else{
			map.insert(md.filepath(), idx);
		}

		idx++;
	}

	sp_log(Log::Debug, this) << "Will delete " << to_delete.size() << " tracks";
	success = deleteTracks(to_delete);
	sp_log(Log::Debug, this) << "delete tracks: " << success;

	success = deleteTracks(v_md_update);
	sp_log(Log::Debug, this) << "delete other tracks: " << success;

	success = db_library.storeMetadata(v_md_update);
	sp_log(Log::Debug, this) << "update tracks: " << success;

	return false;
}

QStringList DatabaseTracks::getAllGenres()
{
	QString querystring;
	bool success;

	SayonaraQuery q(_db);

	querystring = "SELECT genre FROM tracks GROUP BY genre;";
	q.prepare(querystring);

	success = q.exec();
	if(!success){
		return QStringList();
	}

	QHash<QString, bool> hash;
	while(q.next()){
		QString genre = q.value("genre").toString();
		QStringList subgenres = genre.split(",");

		for(const QString& g: subgenres){
			hash[g.toLower()] = true;
		}
	}

	return QStringList(hash.keys());
}


void DatabaseTracks::updateTrackCissearch()
{
	DatabaseSearchMode::update_search_mode();

	MetaDataList v_md;
	getAllTracks(v_md);

	_db.transaction();
	for(const MetaData& md : v_md) {
		QString querystring = "UPDATE tracks SET cissearch=:cissearch WHERE trackID=:id;";
		SayonaraQuery q(_db);
		q.prepare(querystring);
		q.bindValue(":cissearch", Library::convert_search_string(md.title, search_mode()));
		q.bindValue(":id", md.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}
	_db.commit();
}


bool DatabaseTracks::updateTrack(const MetaData& md)
{
	if(md.id < 0 || md.album_id < 0 || md.artist_id < 0 || md.library_id < 0) {
		sp_log(Log::Warning, this) << "Cannot update track: "
								   << " ArtistID: " << md.artist_id
								   << " AlbumID: " << md.album_id
								   << " TrackID: " << md.id
								   << " LibraryID: " << md.library_id;
		return false;
	}

	SayonaraQuery q(_db);

	QString cissearch = Library::convert_search_string(md.title, search_mode());

	q.prepare("UPDATE Tracks "
			  "SET albumID=:albumID, "
			  "artistID=:artistID, "
			  "albumID=:albumID, "
			  "albumArtistID=:albumArtistID, "
			  "title=:title, "
			  "year=:year, "
			  "length=:length, "
			  "bitrate=:bitrate, "
			  "track=:track, "
			  "genre=:genre, "
			  "filesize=:filesize, "
			  "discnumber=:discnumber, "
			  "cissearch=:cissearch, "
			  "rating=:rating, "
			  "modifydate=:modifydate, "
			  "libraryID=:libraryID "
			  "WHERE TrackID = :trackID;");

	q.bindValue(":albumID",			md.album_id);
	q.bindValue(":artistID",		md.artist_id);
	q.bindValue(":albumArtistID",	md.album_artist_id());
	q.bindValue(":title",			md.title);
	q.bindValue(":track",			md.track_num);
	q.bindValue(":length",			md.length_ms);
	q.bindValue(":bitrate",			md.bitrate);
	q.bindValue(":year",			md.year);
	q.bindValue(":trackID",			md.id);
	q.bindValue(":genre",			md.genres_to_string());
	q.bindValue(":filesize",		md.filesize);
	q.bindValue(":discnumber",		md.discnumber);
	q.bindValue(":cissearch",		cissearch);
	q.bindValue(":rating",			md.rating);
	q.bindValue(":modifydate",		Helper::current_date_to_int());
	q.bindValue(":libraryID",		md.library_id);

	if (!q.exec()) {
		q.show_error(QString("Cannot update track ") + md.filepath());
		return false;
	}

	return true;
}

bool DatabaseTracks::updateTracks(const MetaDataList& lst)
{
	bool success;
	int n_files = 0;

	_db.transaction();
	for(const MetaData& md : lst){
		if(updateTrack(md)){
			n_files++;
		}
	}

	success = _db.commit();

	return success && (n_files == lst.size());
}

bool DatabaseTracks::insertTrackIntoDatabase(const MetaData& md, int artist_id, int album_id)
{
	return insertTrackIntoDatabase(md, artist_id, album_id, artist_id);
}

bool DatabaseTracks::insertTrackIntoDatabase(const MetaData& md, int artist_id, int album_id, int album_artist_id)
{
	SayonaraQuery q(_db);

	MetaData md_tmp = getTrackByPath( md.filepath() );

	if( md_tmp.id >= 0 ) {
		MetaData track_copy = md;
		track_copy.id = md_tmp.id;
		track_copy.artist_id = artist_id;
		track_copy.album_id = album_id;
		track_copy.set_album_artist_id(album_artist_id);

		return updateTrack(track_copy);
	}

	QString cissearch = Library::convert_search_string(md.title, search_mode());
	QString querytext =
			"INSERT INTO tracks "
			"(filename,  albumID, artistID, albumArtistID,  title,  year,  length,  track,  bitrate,  genre,  filesize,  discnumber,  rating,  cissearch,  createdate,  modifydate,  libraryID) "
			"VALUES "
			"(:filename,:albumID,:artistID, :albumArtistID, :title, :year, :length, :track, :bitrate, :genre, :filesize, :discnumber, :rating, :cissearch, :createdate, :modifydate, :libraryID); ";

	quint64 current_time = Helper::current_date_to_int();
	q.prepare(querytext);

	q.bindValue(":filename",		md.filepath());
	q.bindValue(":albumID",			album_id);
	q.bindValue(":artistID",		artist_id);
	q.bindValue(":albumArtistID",	album_artist_id);
	q.bindValue(":title",			md.title);
	q.bindValue(":year",			md.year);
	q.bindValue(":length",			md.length_ms);
	q.bindValue(":track",			md.track_num);
	q.bindValue(":bitrate",			md.bitrate);
	q.bindValue(":genre",			md.genres_to_string());
	q.bindValue(":filesize",		md.filesize);
	q.bindValue(":discnumber",		md.discnumber);
	q.bindValue(":rating",			md.rating);
	q.bindValue(":cissearch",		cissearch);
	q.bindValue(":createdate",		current_time);
	q.bindValue(":modifydate",		current_time);
	q.bindValue(":libraryID",		md.library_id);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert track into database ") + md.filepath());
		return false;
	}

	return true;
}


bool DatabaseTracks::updateTrackDates()
{
	QString querytext = "SELECT trackID, filename FROM tracks;";
	SayonaraQuery q(_db);
	q.prepare(querytext);
	QMap<int, QString> v_md;

	QList< std::tuple<int, quint64, quint64> > lst;
	if(q.exec())
	{
		while(q.next())
		{
			int id = q.value(0).toInt();
			QString filepath = q.value(1).toString();

			QString dir = Helper::File::get_parent_directory(filepath);
			QFileInfo fi(filepath);
			QFileInfo fi_dir(dir);
			QDateTime created = fi_dir.created();
			QDateTime modified = fi.lastModified();

			lst << std::make_tuple(id, Helper::date_to_int(created), Helper::date_to_int(modified));
		}
	}

	else{
		q.show_error("Insert dates");
		return false;
	}

	_db.transaction();

	for(auto t : lst){
		SayonaraQuery q(_db);
		q.prepare("UPDATE tracks SET createdate=:createdate, modifydate=:modifydate WHERE trackID = :id;");
		q.bindValue(":id", std::get<0>(t));
		q.bindValue(":createdate", std::get<1>(t));
		q.bindValue(":modifydate", std::get<2>(t));
		q.exec();
	}

	_db.commit();

	sp_log(Log::Debug, "Database Tracks") << "Insert dates finished!";
	return true;
}

void DatabaseTracks::change_artistid_field(const QString& field)
{
	_artistid_field = field;
}
