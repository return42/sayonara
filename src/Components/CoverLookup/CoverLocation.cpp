/* CoverLocation.cpp */

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




#include "CoverLocation.h"
#include "CoverHelper.h"
#include "LocalCoverSearcher.h"
#include "Helper/Helper.h"
#include "Database/DatabaseHandler.h"

#include <QDir>


CoverLocation::CoverLocation() {
	valid = false;
}

CoverLocation::CoverLocation(const CoverLocation& cl){
	search_url = cl.search_url;
	cover_path = cl.cover_path;
	local_paths = cl.local_paths;
	valid = cl.valid;
}

QString CoverLocation::get_cover_directory(){
	return Helper::get_sayonara_path() + QDir::separator() + "covers";
}

CoverLocation CoverLocation::getInvalidLocation() {

	CoverLocation cl;
	cl.cover_path = Helper::get_share_path() + "logo.png";
	cl.search_url = "";
	cl.valid = false;
	return cl;
}

void CoverLocation::print() const{

	sp_log(Log::Info) << "CoverLocation: " << cover_path;
	sp_log(Log::Info) << "CoverLocation: " << search_url;
}

QString CoverLocation::toString() const{
    return QString("Location ") + cover_path + " Url: " + search_url;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name) {

	QString cover_dir = get_cover_directory();
	CoverLocation ret;
	QString cover_token = CoverHelper::calc_cover_token(artist_name, album_name);

	QString cover_path =  cover_dir + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	ret.cover_path = cover_path;
	ret.search_url = CoverHelper::calc_google_album_address(artist_name, album_name);
	ret.valid = true;

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists) {

	QString major_artist = Helper::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}

CoverLocation CoverLocation::get_cover_location(int album_id, quint8 db_id) {

	if(album_id < 0) {
		return CoverLocation::getInvalidLocation();
	}

	Album album;
	MetaDataList v_md;

	CoverLocation cl;

	LibraryDatabase* db = DB::getInstance(db_id);

	bool success = db->getAlbumByID(album_id, album);

	if(!success) {
		return getInvalidLocation();
	}

	cl = get_cover_location(album);

	db->getAllTracksByAlbum(album_id, v_md);
	for(const MetaData& md : v_md){
		cl.local_paths = LocalCoverSearcher::get_local_cover_paths_from_filename(md.filepath());
		if(!cl.local_paths.isEmpty()){
			break;
		}
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const Album& album) {
	int n_artists;

	n_artists = album.artists.size();

	CoverLocation cl;

	if( n_artists > 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists);
	}

	else if( n_artists == 1 ) {
		cl = CoverLocation::get_cover_location(album.name, album.artists[0]);
	}

	else {
		cl = CoverLocation::get_cover_location(album.name, "");
	}

	if(!album.cover_download_url.isEmpty()){
		cl.search_url = album.cover_download_url;
	}

	return cl;
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist.name);

	if(!artist.cover_download_url.isEmpty()){
		cl.search_url = artist.cover_download_url;
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QString& artist) {

	if(artist.isEmpty()) return getInvalidLocation();

	QString cover_dir = get_cover_directory();
	CoverLocation ret;

	QString token = QString("artist_") + CoverHelper::calc_cover_token(artist, "");
	QString target_file = cover_dir + QDir::separator() + token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	ret.cover_path = target_file;
	ret.search_url = CoverHelper::calc_google_artist_address(artist);
	ret.valid = true;

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md) {

    CoverLocation cl;
    if(md.album_id >= 0){
		cl = get_cover_location(md.album_id, md.db_id);
    }

	if(!cl.valid){
		cl = get_cover_location(md.album, md.artist);
	}

	if(!md.cover_download_url.isEmpty() && cl.search_url.contains("google")){
		cl.search_url = md.cover_download_url;
	}

	if(cl.search_url.isEmpty()){
		cl.search_url = md.cover_download_url;
	}

	return cl;
}

