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
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/Logger/Logger.h"
#include "Helper/globals.h"

#include "Database/DatabaseHandler.h"

#include <QDir>
#include <QUrl>
#include <QStringList>

struct CoverLocation::Private
{
	QString			search_term;  // Term provided to search engine
	QString			search_url; // Search url where to fetch covers
	QString			cover_path;  // cover_path path, in .Sayonara, where cover is stored. Ignored if local_paths are not empty
	QStringList		local_paths; // local_paths paths where images can be fetched from if they should not be fetched from the .Sayonara directory
	bool			valid; // valid if CoverLocation object contains a valid download url
};

CoverLocation::CoverLocation() {
	qRegisterMetaType<CoverLocation>("CoverLocation");

	_m = new CoverLocation::Private();
	_m->valid = false;
}

CoverLocation::CoverLocation(const CoverLocation& cl) : 
	CoverLocation()
{
	_m->search_url = cl.search_url();
	_m->search_term = cl.search_term();
	_m->cover_path = cl.cover_path();
	_m->local_paths = cl.local_paths();
	_m->valid = cl.valid();
}

CoverLocation::~CoverLocation()
{
	delete _m; _m = nullptr;
}

QString CoverLocation::get_cover_directory(){
	return Helper::get_sayonara_path() + QDir::separator() + "covers";
}

CoverLocation CoverLocation::getInvalidLocation() 
{
	CoverLocation cl;
	cl._m->cover_path = Helper::get_share_path() + "logo.png";
	cl._m->search_url = "";
	cl._m->search_term = "";
	cl._m->valid = false;
	return cl;
}

bool CoverLocation::isInvalidLocation(const QString& cover_path)
{
	QString path1 = Helper::File::clean_filename(cover_path);
	QString path2 = getInvalidLocation().cover_path();

	return (path1 == path2);
}

void CoverLocation::print() const{

	sp_log(Log::Info) << "CoverLocation: " << _m->cover_path;
	sp_log(Log::Info) << "CoverLocation: " << _m->search_url;
}

QString CoverLocation::toString() const{
	return QString("Location ") + _m->cover_path + " Url: " + _m->search_url;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QString& artist_name) {

	QString cover_dir = get_cover_directory();

	QString cover_token = CoverHelper::calc_cover_token(artist_name, album_name);

	QString cover_path =  cover_dir + QDir::separator() + cover_token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	CoverLocation ret;
	ret._m->cover_path = cover_path;
	ret._m->search_term = artist_name + " " + album_name;
	ret._m->search_url = CoverHelper::calc_google_album_address(artist_name, album_name);
	ret._m->valid = true;

	return ret;
}

CoverLocation CoverLocation::get_cover_location(const QString& album_name, const QStringList& artists) {

	QString major_artist = ArtistList::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}

CoverLocation CoverLocation::get_cover_location(int album_id, quint8 db_id) {

	if(album_id < 0) {
		return CoverLocation::getInvalidLocation();
	}

	Album album;
	MetaDataList v_md;
	LibraryDatabase* db = DB::getInstance(db_id);

	bool success = db->getAlbumByID(album_id, album);

	if(!success) {
		return getInvalidLocation();
	}

	CoverLocation cl;
	cl = get_cover_location(album);

	db->getAllTracksByAlbum(album_id, v_md);
	for(const MetaData& md : v_md){
		cl._m->local_paths = LocalCoverSearcher::get_local_cover_paths_from_filename(md.filepath());
		if(!cl._m->local_paths.isEmpty()){
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
		cl._m->search_url = album.cover_download_url;
	}

	cl._m->search_term = album.name + " " + ArtistList::get_major_artist(album.artists);

	return cl;
}

CoverLocation CoverLocation::get_cover_location(const Artist& artist) {

	CoverLocation cl = CoverLocation::get_cover_location(artist.name);

	if(!artist.cover_download_url.isEmpty()){
		cl._m->search_url = artist.cover_download_url;
	}

	cl._m->search_term = artist.name;

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QString& artist) {

	if(artist.isEmpty()) return getInvalidLocation();

	QString cover_dir = get_cover_directory();
	QString token = QString("artist_") + CoverHelper::calc_cover_token(artist, "");
	QString target_file = cover_dir + QDir::separator() + token + ".jpg";

	if(!QFile::exists(cover_dir)) {
		QDir().mkdir(cover_dir);
	}

	CoverLocation ret;
	ret._m->cover_path = target_file;
	ret._m->search_url = CoverHelper::calc_google_artist_address(artist);
	ret._m->search_term = artist;
	ret._m->valid = true;

	return ret;
}


CoverLocation CoverLocation::get_cover_location(const MetaData& md) {

    CoverLocation cl;
	if(md.album_id >= 0){
		cl = get_cover_location(md.album_id, md.db_id);
	}

	if(!cl.valid()){
		cl = get_cover_location(md.album, md.artist);
	}

	if(!md.cover_download_url.isEmpty())
	{
		QString extension = Helper::File::get_file_extension(md.cover_download_url);
		QString cover_dir = get_cover_directory();
		QString cover_token = CoverHelper::calc_cover_token(md.artist, md.album);
		QString cover_path =  cover_dir + QDir::separator() + cover_token + "." + extension;

		cl = get_cover_location(QUrl(md.cover_download_url), cover_path);
	}

	if(cl._m->search_url.isEmpty()){
		cl._m->search_url = md.cover_download_url;
	}

	return cl;
}


CoverLocation CoverLocation::get_cover_location(const QUrl& url, const QString& target_path)
{
	CoverLocation cl;
	cl._m->cover_path = target_path;
	cl._m->search_url = url.toString();
	cl._m->valid = true;

	return cl;
}


CoverLocation CoverLocation::get_cover_location_by_searchstring(const QString& search_string, const QString& target_path)
{
	CoverLocation cl;
	cl._m->cover_path = target_path;
	cl._m->search_term = search_string;
	cl._m->search_url = CoverHelper::calc_google_image_search_address(search_string);
	cl._m->valid = true;

	return cl;
}


bool CoverLocation::valid() const {
	return _m->valid;
}

QStringList CoverLocation::local_paths() const {
	return _m->local_paths;
}

QString CoverLocation::local_path(int idx) const
{
	if(!between(idx, _m->local_paths)){
		return QString();
	}

	return _m->local_paths[idx];
}

QString CoverLocation::cover_path() const {
	return _m->cover_path;
}

QString CoverLocation::search_url() const {
	return _m->search_url;
}

QString CoverLocation::search_term() const {
	return _m->search_term;
}

void CoverLocation::set_search_term(const QString& search_term)
{
	_m->search_term = search_term;
	_m->search_url = CoverHelper::calc_google_image_search_address(search_term);

}
