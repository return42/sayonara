/* DatabaseConnector.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General License for more details.

 * You should have received a copy of the GNU General License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Database/SayonaraQuery.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"
#include "Database/LocalLibraryDatabase.h"

#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/Artist.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Utils.h"
#include "Utils/FileUtils.h"

#include <QFileInfo>
#include <QDateTime>
#include <QTime>

#include <tuple>
#include <algorithm>

using DB::Connector;

struct Connector::Private
{
	QList<LibraryDatabase*> library_dbs;
	LocalLibraryDatabase*	local_library_database=nullptr;
};

Connector::Connector() :
	DB::Base(0, "", QString("player.db"), nullptr),
	DB::Bookmarks(db(), db_id()),
	DB::Playlist(db(), db_id()),
	DB::Podcasts(db(), db_id()),
	DB::Settings(db(), db_id()),
	DB::Streams(db(), db_id()),
	DB::VisualStyles(db(), db_id())
{
	m = Pimpl::make<Private>();
	apply_fixes();

	m->local_library_database = new DB::LocalLibraryDatabase(-1);
	m->library_dbs << m->local_library_database;
}

Connector::~Connector() {}

void Connector::add_library_db(LibraryDatabase* lib_db)
{
	m->library_dbs << lib_db;
}

bool Connector::updateAlbumCissearchFix()
{
#ifdef DEBUG_DB
	sp_log(Log::Debug, this) << Q_FUNC_INFO;
#endif

	AlbumList albums;

	LibraryDatabase* lib_db = library_db(-1, 0);
	lib_db->getAllAlbums(albums);

	for(const Album& album : albums)
	{
		QString str = "UPDATE albums SET cissearch=:cissearch WHERE albumID=:id;";
		Query q(db());
		q.prepare(str);
		q.bindValue(":cissearch", album.name().toLower());
		q.bindValue(":id", album.id);

		if(!q.exec()){
			q.show_error("Cannot update album cissearch");
		}
	}

	return true;
}


bool Connector::updateArtistCissearchFix()
{
	ArtistList artists;
	LibraryDatabase* lib_db = library_db(-1, 0);
	lib_db->getAllArtists(artists);
	for(const Artist& artist : artists)
	{
		QString str =
				"UPDATE artists SET cissearch=:cissearch WHERE artistID=:id;";

		Query q(db());
		q.prepare(str);
		q.bindValue(":cissearch", artist.name().toLower());
		q.bindValue(":id", artist.id);

		if(!q.exec()){
			q.show_error("Cannot update artist cissearch");
		}
	}

	return true;
}

bool Connector::updateTrackCissearchFix()
{
	MetaDataList v_md;
	LibraryDatabase* lib_db = library_db(-1, 0);
	lib_db->getAllTracks(v_md);
	for(const MetaData& md : v_md) {
		lib_db->updateTrack(md);
	}

	return true;
}

bool Connector::apply_fixes()
{
	QString str_version;
	int version;
	bool success;
	const int LatestVersion = 14;

	success = load_setting("version", str_version);
	version = str_version.toInt(&success);
	sp_log(Log::Info, this)
			<< "Database Version:  " << version << ". "
			<< "Latest Version: " << LatestVersion;

	if(version == LatestVersion) {
		sp_log(Log::Info, this) << "No need to update db";
		return true;
	}

	else if(!success){
		 sp_log(Log::Warning, this) << "Cannot get database version";
	}

	sp_log(Log::Info, this) << "Apply fixes";

	if(version < 1)
	{
		check_and_insert_column("playlisttotracks", "position", "INTEGER");
		check_and_insert_column("playlisttotracks", "filepath", "VARCHAR(512)");
		check_and_insert_column("tracks", "genre", "VARCHAR(1024)");

		QString create_savedstreams = QString("CREATE TABLE savedstreams ") +
				"( " +
				"	name VARCHAR(255) PRIMARY KEY, " +
				"	url VARCHAR(255) " +
				");";

		check_and_create_table("savedstreams", create_savedstreams);


		QString create_savedpodcasts = QString("CREATE TABLE savedpodcasts ") +
				"( " +
				"	name VARCHAR(255) PRIMARY KEY, " +
				"	url VARCHAR(255) " +
				");";

		check_and_create_table("savedpodcasts", create_savedpodcasts);
	}

	if(version < 3) {
		db().transaction();

		bool success = true;
		success &= check_and_insert_column("tracks", "cissearch", "VARCHAR(512)");
		success &= check_and_insert_column("albums", "cissearch", "VARCHAR(512)");
		success &= check_and_insert_column("artists", "cissearch", "VARCHAR(512)");

		Q_UNUSED(success)

		updateAlbumCissearchFix();
		updateArtistCissearchFix();
		updateTrackCissearchFix();

		db().commit();
	}


	if(version == 3) {
		check_and_drop_table("VisualStyles");
	}

	if(version < 4) {
		QString create_vis_styles = QString("CREATE TABLE VisualStyles ") +
				"( " +
				"  name VARCHAR(255) PRIMARY KEY, " +
				"  col1 VARCHAR(20), "
				"  col2 VARCHAR(20), "
				"  col3 VARCHAR(20), "
				"  col4 VARCHAR(20), "
				"  nBinsSpectrum INTEGER, "
				"  rectHeightSpectrum INTEGER, "
				"  fadingStepsSpectrum INTEGER, "
				"  horSpacingSpectrum INTEGER, "
				"  vertSpacingSpectrum INTEGER, "
				"  rectWidthLevel INTEGER, "
				"  rectHeightLevel INTEGER, "
				"  horSpacingLevel INTEGER, "
				"  verSpacingLevel INTEGER, "
				"  fadingStepsLevel INTEGER "
				");";

		bool success = check_and_create_table("VisualStyles", create_vis_styles);
		if(success) store_setting("version", 4);
	}

	if(version < 5) {
		bool success = check_and_insert_column("tracks", "rating", "integer");
		if(success) store_setting("version", 5);
	}

	if(version < 6) {
		QString create_savedbookmarks = QString("CREATE TABLE savedbookmarks ") +
				"( " +
				"	trackid INTEGER, " +
				"	name VARCHAR(255), " +
				"	timeidx INTEGER, " +
				"   PRIMARY KEY (trackid, timeidx), " +
				"   FOREIGN KEY (trackid) REFERENCES tracks(trackid) " +
				");";

		bool success = check_and_create_table("savedbookmarks", create_savedbookmarks);
		if(success) store_setting("version", 6);
	}

	if(version < 7) {
		bool success = check_and_insert_column("albums", "rating", "integer");
		if(success) store_setting("version", 7);
	}

	if(version < 9) {
		bool success = check_and_insert_column("playlists", "temporary", "integer");

		if(success) {
			Query q(db());
			QString querytext = "UPDATE playlists SET temporary=0;";
			q.prepare(querytext);
			if(q.exec()){
				store_setting("version", 9);
			};
		}
	}

	if(version < 10){
		bool success = check_and_insert_column("playlisttotracks", "db_id", "integer");
		if(success) {
			Query q(db());
			Query q_index(db());
			QString querytext = "UPDATE playlisttotracks SET db_id = (CASE WHEN trackid > 0 THEN 0 ELSE -1 END)";
			QString index_query = "CREATE INDEX album_search ON albums(cissearch, albumID);"
					"CREATE INDEX artist_search ON artists(cissearch, artistID);"
					"CREATE INDEX track_search ON tracks(cissearch, trackID);";

			q.prepare(querytext);
			q_index.prepare(index_query);

			if(q.exec()){
				store_setting("version", 10);
			};

			q_index.exec();
		}
	}

	if(version < 11)
	{
		// look in UpdateDatesThread
	}

	if(version < 12){
		QString querytext =
				"CREATE VIEW album_info_view AS "
				"SELECT "
				"	albums.albumID as albumID, "
				"	albums.name as name, "
				"	albums.cissearch as cissearch, "
				"	albums.rating as rating, "
				"	COUNT(artists.artistID) as artistCount, "
				"	COUNT(tracks.trackID) as trackCount, "
				"	CASE WHEN COUNT(DISTINCT artists.artistID) > 1 "
				"	THEN 1 "
				"	ELSE 0 "
				"	END as Sampler "
				"FROM albums, artists, tracks "
				"WHERE albums.albumID = tracks.albumID "
				"AND artists.artistID = tracks.artistID "
				"GROUP BY albums.albumID, albums.name";
			;

		Query q(db());
		q.prepare(querytext);

		if(q.exec()){
			store_setting("version", 12);
		}
	}

	if(version < 13){
		bool success = check_and_insert_column("tracks", "albumArtistID", "integer", "-1");

		Query q(db());
		q.prepare("UPDATE tracks SET albumArtistID=artistID;");
		success = success && q.exec();

		if(success){
			store_setting("version", 13);
		}
	}

	if(version < 14){
		bool success=check_and_insert_column("tracks", "libraryID", "integer", "0");
		Query q(db());
		q.prepare("UPDATE tracks SET libraryID=0;");
		success = success && q.exec();

		if(success){
			store_setting("version", 14);
		}
	}

	return true;
}


void Connector::clean_up()
{
	Query q(db());
	QString querytext = "VACUUM;";
	q.prepare(querytext);
	q.exec();
}

DB::LibraryDatabases Connector::library_dbs() const
{
	return m->library_dbs;
}


DB::LibraryDatabase* Connector::library_db(int8_t library_id, uint8_t db_id)
{
	for(LibraryDatabase* db : m->library_dbs)
	{
		if( (db->library_id() == library_id) &&
			(db->db_id() == db_id))
		{
			return db;
		}
	}

	sp_log(Log::Warning, this) << "Could not find Library:"
								" DB ID = " << (int) db_id
							 << " LibraryID = " << (int) library_id;

	return m->local_library_database;
}

DB::LibraryDatabase *Connector::find_library_db(int8_t library_id) const
{
	for(DB::LibraryDatabase* db : m->library_dbs)
	{
		if( (db->library_id() == library_id)){
			return db;
		}
	}

	return nullptr;
}

DB::LibraryDatabase* Connector::register_library_db(int8_t library_id)
{
	DB::LibraryDatabase* lib_db = find_library_db(library_id);

	if(!lib_db) {
		lib_db = new DB::LocalLibraryDatabase(library_id);
		add_library_db(lib_db);
	}

	return lib_db;
}
