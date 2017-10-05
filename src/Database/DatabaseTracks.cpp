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

#include <QFileInfo>
#include <QDateTime>
#include <QMap>

#include <utility>
#include <tuple>

struct DatabaseTracks::Private
{
	QString track_view_name;
	QString track_search_view_name;
	QString artistid_field;
	int8_t library_id;
};

DatabaseTracks::DatabaseTracks(const QSqlDatabase& db, uint8_t db_id, int8_t library_id) :
	DatabaseSearchMode(db, db_id)
{
	m = Pimpl::make<Private>();

	m->artistid_field = "artistID";
	m->library_id = library_id;

	check_track_view(library_id);
}

DatabaseTracks::~DatabaseTracks() {}


void DatabaseTracks::check_track_view(int8_t library_id)
{
	if(library_id < 0) {
		m->track_view_name = QString("tracks");
		m->track_search_view_name = QString("track_search_view");
	}

	else {
		m->track_view_name = QString("track_view_%1").arg(library_id);
		m->track_search_view_name = QString("track_search_view_%1").arg(library_id);
	}

	QString select = "SELECT "
					"trackID, "									// 0
					"title, "									// 1
					"length, "									// 2
					"year, "									// 3
					"bitrate, "									// 4
					"filename, "								// 5
					"filesize, "								// 6
					"track AS trackNum, "						// 7
					"genre, "									// 8
					"discnumber, "								// 9
					"tracks.rating, "							// 10
					"tracks.albumID AS albumID, "				// 11
					"tracks.artistID AS artistID, "				// 12
					"tracks.albumArtistID AS albumArtistID, "	// 13
					"createDate, "								// 14
					"modifyDate, "								// 15
					"tracks.libraryID AS trackLibraryID "		// 16
	;

	QString view_query =
					"CREATE "
					"VIEW "
					"IF NOT EXISTS "
					+ m->track_view_name + " "
					"AS " + select + " "
					"FROM tracks "
	;

	QString search_view_query =
			"CREATE "
			"VIEW "
			"IF NOT EXISTS "
			+ m->track_search_view_name + " "
			"AS "
			+ select + ", "
			"albums.name AS albumName, "				// 17
			"albums.rating AS albumRating, "			// 18
			"artists.name AS artistName, "				// 19
			"albumArtists.name AS albumArtistName, "	// 20
			"(albums.cissearch || ',' || artists.cissearch || ',' || tracks.cissearch) AS allCissearch " // 21
			"FROM tracks "
			"INNER JOIN albums ON tracks.albumID = albums.albumID "
			"INNER JOIN artists ON tracks.artistID = artists.artistID "
			"LEFT OUTER JOIN artists albumArtists ON tracks.albumArtistID = albumArtists.artistID ";
	;

	if(m->library_id >= 0){
		view_query += "WHERE libraryID=" + QString::number(m->library_id) + "; ";
		search_view_query += "WHERE libraryID=" + QString::number(m->library_id) + "; ";
	}

	SayonaraQuery view_q(this);
	SayonaraQuery search_view_q(this);

	view_q.prepare(view_query);
	search_view_q.prepare(search_view_query);

	if(library_id >= 0){
		if(!view_q.exec())
		{
			view_q.show_error("Cannot create track view");
		}
	}

	if(!search_view_q.exec())
	{
		search_view_q.show_error("Cannot create track search view");
	}
}

QString DatabaseTracks::fetch_query_tracks() const
{
	return "SELECT * FROM " + m->track_search_view_name + " ";
}


bool DatabaseTracks::db_fetch_tracks(SayonaraQuery& q, MetaDataList& result)
{
	result.clear();

	if (!q.exec()) {
		q.show_error("Cannot fetch tracks from database");
		return false;
	}

	result.reserve(q.fetched_rows());

	while(q.next())
	{
		MetaData data;

		data.id = 		 	q.value(0).toInt();
		data.title = 	 	q.value(1).toString();
		data.length_ms = 	q.value(2).toInt();
		data.year = 	 	q.value(3).toInt();
		data.bitrate = 	 	q.value(4).toInt();
		data.set_filepath(q.value(5).toString());
		data.filesize =  	q.value(6).toInt();
		data.track_num = 	q.value(7).toInt();
		data.set_genres(q.value(8).toString().split(","));
		data.discnumber = 	q.value(9).toInt();
		data.rating = 		q.value(10).toInt();
		data.album_id =  	q.value(11).toInt();
		data.set_album(q.value(17).toString().trimmed());
		data.artist_id = 	q.value(12).toInt();
		data.set_artist(q.value(19).toString().trimmed());
		data.set_album_artist(q.value(20).toString(), q.value(13).toInt());
		data.library_id = 	q.value(16).toInt();
		data.set_db_id(module_db_id());

		result.push_back(std::move(data));
	}

	return true;
}


QString DatabaseTracks::append_track_sort_string(QString querytext, Library::SortOrder sort)
{
	if(sort == Library::SortOrder::TrackArtistAsc) querytext += QString(" ORDER BY artistName ASC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackArtistDesc) querytext += QString(" ORDER BY artistName DESC, discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackAlbumAsc) querytext += QString(" ORDER BY discnumber ASC, albumName ASC, trackNum;");
	else if(sort == Library::SortOrder::TrackAlbumDesc) querytext += QString(" ORDER BY discnumber ASC, albumName DESC, trackNum;");
	else if(sort == Library::SortOrder::TrackTitleAsc) querytext += QString(" ORDER BY title ASC;");
	else if(sort == Library::SortOrder::TrackTitleDesc) querytext += QString(" ORDER BY title DESC;");
	else if(sort == Library::SortOrder::TrackNumAsc) querytext += QString(" ORDER BY trackNum ASC;");
	else if(sort == Library::SortOrder::TrackNumDesc) querytext += QString(" ORDER BY trackNum DESC;");
	else if(sort == Library::SortOrder::TrackYearAsc) querytext += QString(" ORDER BY year ASC;");
	else if(sort == Library::SortOrder::TrackYearDesc) querytext += QString(" ORDER BY year DESC;");
	else if(sort == Library::SortOrder::TrackLenghtAsc) querytext += QString(" ORDER BY length ASC;");
	else if(sort == Library::SortOrder::TrackLengthDesc) querytext += QString(" ORDER BY length DESC;");
	else if(sort == Library::SortOrder::TrackBitrateAsc) querytext += QString(" ORDER BY bitrate ASC;");
	else if(sort == Library::SortOrder::TrackBitrateDesc) querytext += QString(" ORDER BY bitrate DESC;");
	else if(sort == Library::SortOrder::TrackSizeAsc) querytext += QString(" ORDER BY filesize ASC;");
	else if(sort == Library::SortOrder::TrackSizeDesc) querytext += QString(" ORDER BY filesize DESC;");
	else if(sort == Library::SortOrder::TrackRatingAsc) querytext += QString(" ORDER BY rating ASC;");
	else if(sort == Library::SortOrder::TrackRatingDesc) querytext += QString(" ORDER BY rating DESC;");

	else querytext += ";";

	return querytext;
}


bool DatabaseTracks::getMultipleTracksByPath(const QStringList& paths, MetaDataList& v_md)
{
	module_db().transaction();

	for(const QString& path : paths) {
		v_md << getTrackByPath(path);
	}

	module_db().commit();

	return (v_md.count() == paths.size());
}


MetaData DatabaseTracks::getTrackByPath(const QString& path)
{
	SayonaraQuery q(this);

	QString querytext = fetch_query_tracks() +
						"WHERE filename LIKE :filename;";
	q.prepare(querytext);
	q.bindValue(":filename", path);

	MetaData md(path);
	md.set_db_id(module_db_id());

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
	SayonaraQuery q(this);
	QString querytext = fetch_query_tracks() + "WHERE trackID = :track_id;";

	q.prepare(querytext);
	q.bindValue(":track_id", id);

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
	SayonaraQuery q(this);

	QString querytext = fetch_query_tracks();
	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);

	return db_fetch_tracks(q, returndata);
}


bool DatabaseTracks::getAllTracksByAlbum(int album, MetaDataList& result)
{
	return getAllTracksByAlbum(album, result, Library::Filter());
}


bool DatabaseTracks::getAllTracksByAlbum(int album, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort, int discnumber)
{
	MetaDataList v_md;

	IDList list{album};
	returndata.clear();

	bool success = getAllTracksByAlbum(list, v_md, filter, sort);

	if(discnumber < 0) {
		returndata = v_md;
	}

	for(const MetaData& md : v_md)
	{
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
	if(albums.isEmpty()) {
		return false;
	}

	SayonaraQuery q(this);

	QString querytext = fetch_query_tracks();

	if( !filter.cleared() )
	{
		switch( filter.mode() )
		{
			case Library::Filter::Genre:
				querytext += "WHERE genre LIKE :searchterm AND ";
				break;

			case Library::Filter::Filename:
				querytext += "WHERE filename LIKE :searchterm AND ";
				break;

			case Library::Filter::Fulltext:
			default:
				querytext += "WHERE allCissearch LIKE :searchterm AND ";
				break;
		}
	}

	else{
		querytext += " WHERE ";
	}

	if(albums.size() > 0) {
		QString album_id_field = m->track_search_view_name + ".albumID ";
		querytext += " (" + album_id_field + "=:albumid_0 ";
		for(int i=1; i<albums.size(); i++) {
			querytext += "OR " + album_id_field + "=:albumid_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}


	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);

	for(int i=0; i<albums.size(); i++) {
		q.bindValue(QString(":albumid_") + QString::number(i), albums[i]);
	}

	if( !filter.cleared() )
	{
		QString filtertext = filter.filtertext();
        q.bindValue(":searchterm", filtertext);
	}

	return db_fetch_tracks(q, returndata);
}

bool DatabaseTracks::getAllTracksByArtist(int artist, MetaDataList& returndata)
{
	return getAllTracksByArtist(artist, returndata, Library::Filter());
}

bool DatabaseTracks::getAllTracksByArtist(int artist, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort)
{
	IDList list{artist};

	return getAllTracksByArtist(list, returndata, filter, sort);
}

bool DatabaseTracks::getAllTracksByArtist(IDList artists, MetaDataList& returndata)
{
	return 	getAllTracksByArtist(artists, returndata, Library::Filter());
}


bool DatabaseTracks::getAllTracksByArtist(IDList artists, MetaDataList& returndata, const Library::Filter& filter, Library::SortOrder sort)
{
	if(artists.empty()){
		return false;
	}

	SayonaraQuery q(this);

	QString querytext = fetch_query_tracks();
	if( !filter.cleared() )
	{
		switch( filter.mode() )
		{
			case Library::Filter::Genre:
				querytext += "WHERE genre LIKE :searchterm AND ";
				break;

			case Library::Filter::Filename:
				querytext += "WHERE filename LIKE :searchterm AND ";
				break;

			case Library::Filter::Fulltext:
			default:
				querytext += "WHERE allCissearch LIKE :searchterm AND ";
				break;
		}
	}

	else{
		querytext += " WHERE ";
	}

	if(artists.size() > 0)
	{
		QString artist_id_field = m->track_search_view_name + "." + m->artistid_field;
		querytext += " (" + artist_id_field + "=:artist_id_0 ";
		for(int i=1; i<artists.size(); i++) {
			querytext += "OR " + artist_id_field + "=:artist_id_" + QString::number(i) + " ";
		}

		querytext += ") ";
	}

	querytext = append_track_sort_string(querytext, sort);

	q.prepare(querytext);
	q.bindValue(":artist_id", QVariant(artists.first()));

	for(int i=0; i<artists.size(); i++) {
		q.bindValue(QString(":artist_id_") + QString::number(i), artists[i]);
	}


	q.bindValue(":searchterm", filter.filtertext());

	return db_fetch_tracks(q, returndata);
}

bool DatabaseTracks::getAllTracksBySearchString(const Library::Filter& filter, MetaDataList& result, Library::SortOrder sort)
{
	SayonaraQuery q(this);

	QString querytext = fetch_query_tracks();

	switch(filter.mode())
	{
		case Library::Filter::Genre:
			querytext += "WHERE genre LIKE :searchterm ";
			break;

		case Library::Filter::Filename:
			querytext += "WHERE filename LIKE :searchterm ";
			break;

		case Library::Filter::Fulltext:
			querytext += "WHERE allCissearch LIKE :searchterm ";
			break;

		default:
			return false;
	}

	querytext = append_track_sort_string(querytext, sort);
	q.prepare(querytext);

	QString filtertext = filter.filtertext();
	q.bindValue(":searchterm", filtertext);

	return db_fetch_tracks(q, result);
}



bool DatabaseTracks::deleteTrack(int id)
{
	SayonaraQuery q(this);
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

	module_db().transaction();

	for(const int& id : ids){
		if( deleteTrack(id) ){
			n_files++;
		};
	}

	bool success = module_db().commit();

	return (success && (n_files == ids.size()));
}


bool DatabaseTracks::deleteTracks(const MetaDataList& v_md)
{
	module_db().transaction();

	size_t deleted_tracks = std::count_if(v_md.begin(), v_md.end(), [=](const MetaData& md)
	{
		return this->deleteTrack(md.id);
	});

	module_db().commit();

	sp_log(Log::Info) << "Deleted " << deleted_tracks << " of " << v_md.size() << " tracks";

	return (deleted_tracks == v_md.size());
}

bool DatabaseTracks::deleteInvalidTracks(const QString& library_path)
{
	bool success;

	MetaDataList v_md;
	QMap<QString, int> map;
	IDList to_delete;
	MetaDataList v_md_update;

	SayonaraQuery q(this);
	DatabaseLibrary db_library(module_db(), module_db_id(), m->library_id);

	if(!getAllTracks(v_md)){
		sp_log(Log::Error) << "Cannot get tracks from db";
		return false;
	}

	int idx = 0;
	for(const MetaData& md : v_md)
	{
		if(map.contains(md.filepath())) {
			sp_log(Log::Warning, this) << "found double path: " << md.filepath();
			int old_idx = map[md.filepath()];
			to_delete << md.id;
			v_md_update << v_md[old_idx];
		}

		else {
			map.insert(md.filepath(), idx);
		}

		if( (!library_path.isEmpty()) &&
			(!md.filepath().contains(library_path)) )
		{
			to_delete << md.id;
		}

		idx++;
	}

	sp_log(Log::Debug, this) << "Will delete " << to_delete.size() << " double-tracks";
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

	SayonaraQuery q(this);

	querystring = "SELECT genre FROM " + m->track_view_name + " GROUP BY genre;";
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

	module_db().transaction();

	for(const MetaData& md : v_md)
	{
		QString querystring = "UPDATE tracks SET cissearch=:cissearch WHERE trackID=:id;";
		SayonaraQuery q(this);
		q.prepare(querystring);
		q.bindValue(":cissearch", Library::convert_search_string(md.title, search_mode()));
		q.bindValue(":id", md.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	module_db().commit();
}

void DatabaseTracks::deleteAllTracks()
{
	if(m->library_id >= 0)
	{
		SayonaraQuery q(this);
		q.prepare("DROP VIEW " + m->track_search_view_name + ";");
		//q.exec();

		SayonaraQuery q2(this);
		q2.prepare("DELETE FROM tracks WHERE libraryId=:library_id;");
		q2.bindValue(":library_id", m->library_id);
		q2.exec();
	}
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

	SayonaraQuery q(this);

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
    q.bindValue(":length",			(uint32_t) md.length_ms);
	q.bindValue(":bitrate",			md.bitrate);
	q.bindValue(":year",			md.year);
	q.bindValue(":trackID",			md.id);
	q.bindValue(":genre",			md.genres_to_string());
    q.bindValue(":filesize",		(uint32_t) md.filesize);
	q.bindValue(":discnumber",		md.discnumber);
	q.bindValue(":cissearch",		cissearch);
	q.bindValue(":rating",			md.rating);
	q.bindValue(":modifydate",		(quint64) Helper::current_date_to_int());
	q.bindValue(":libraryID",		md.library_id);

	if (!q.exec()) {
		q.show_error(QString("Cannot update track ") + md.filepath());
		return false;
	}

	return true;
}

bool DatabaseTracks::updateTracks(const MetaDataList& v_md)
{
	module_db().transaction();

	size_t n_files = std::count_if(v_md.begin(), v_md.end(), [=](const MetaData& md){
		return this->updateTrack(md);
	});

	bool success = module_db().commit();

	return success && (n_files == v_md.size());
}

bool DatabaseTracks::insertTrackIntoDatabase(const MetaData& md, int artist_id, int album_id)
{
	return insertTrackIntoDatabase(md, artist_id, album_id, artist_id);
}

bool DatabaseTracks::insertTrackIntoDatabase(const MetaData& md, int artist_id, int album_id, int album_artist_id)
{
	SayonaraQuery q(this);

	MetaData md_tmp = getTrackByPath( md.filepath() );

	if( md_tmp.id >= 0 )
	{
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

	uint64_t current_time = Helper::current_date_to_int();
	q.prepare(querytext);

	q.bindValue(":filename",		md.filepath());
	q.bindValue(":albumID",			album_id);
	q.bindValue(":artistID",		artist_id);
	q.bindValue(":albumArtistID",	album_artist_id);
	q.bindValue(":title",			md.title);
	q.bindValue(":year",			md.year);
	q.bindValue(":length",			(quint64) md.length_ms);
	q.bindValue(":track",			md.track_num);
	q.bindValue(":bitrate",			md.bitrate);
	q.bindValue(":genre",			md.genres_to_string());
	q.bindValue(":filesize",		(quint64) md.filesize);
	q.bindValue(":discnumber",		md.discnumber);
	q.bindValue(":rating",			md.rating);
	q.bindValue(":cissearch",		cissearch);
	q.bindValue(":createdate",		(quint64) current_time);
	q.bindValue(":modifydate",		(quint64) current_time);
	q.bindValue(":libraryID",		md.library_id);

	if (!q.exec()) {
		q.show_error(QString("Cannot insert track into database ") + md.filepath());
		return false;
	}

	return true;
}

void DatabaseTracks::change_artistid_field(const QString& field)
{
	m->artistid_field = field;
}

void DatabaseTracks::change_track_lookup_field(const QString& track_lookup_field)
{
	m->track_search_view_name = track_lookup_field;
}
