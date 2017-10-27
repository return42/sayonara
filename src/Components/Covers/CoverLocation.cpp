/* CoverLocation.cpp */

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

#include "CoverLocation.h"
#include "CoverUtils.h"
#include "CoverFetchManager.h"
#include "LocalCoverSearcher.h"
#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/Artist.h"
#include "Utils/globals.h"
#include "Utils/Logger/Logger.h"

#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"

#include <QDir>
#include <QUrl>
#include <QStringList>
#include <QImage>

using Cover::Location;

struct Location::Private
{
	QString			search_term;  // Term provided to search engine
	QStringList		search_urls; // Search url where to fetch covers
	QString			cover_path;  // cover_path path, in .Sayonara, where cover is stored. Ignored if local_paths are not empty
	QStringList		local_paths; // local_paths paths where images can be fetched from if they should not be fetched from the .Sayonara directory
	QString			identifier;
	bool			valid; // valid if CoverLocation object contains a valid download url

	Private() :
		valid(false)
	{}

	Private(const Private& other) :
		CASSIGN(search_term),
		CASSIGN(search_urls),
		CASSIGN(cover_path),
		CASSIGN(local_paths),
		CASSIGN(identifier),
		CASSIGN(valid)
	{}

	Private& operator=(const Private& other)
	{
		ASSIGN(search_term);
		ASSIGN(search_urls);
		ASSIGN(cover_path);
		ASSIGN(local_paths);
		ASSIGN(identifier);
		ASSIGN(valid);

		return (*this);
	}

	~Private() {}
};


Location::Location()
{
	qRegisterMetaType<Location>("CoverLocation");

	m = Pimpl::make<Location::Private>();
}

Location::~Location() {}

Location::Location(const Location& other)
{
	m = Pimpl::make<Location::Private>(*(other.m));
}

Location& Location::operator=(const Location& other)
{
	*m = *(other.m);
	return *this;
}

QString Location::get_cover_directory(const QString& append_path)
{
	QString cover_dir = ::Util::sayonara_path("covers");
	if(!QFile::exists(cover_dir)){
		QDir().mkdir(cover_dir);
	}

	if(!append_path.isEmpty()){
		cover_dir += "/" + append_path;
	}

	return ::Util::File::clean_filename(cover_dir);
}

QString Location::preferred_path() const
{
	if(!this->local_paths().isEmpty()){
		return this->local_paths().first();
	}

	if(QFile::exists(this->cover_path())){
		return this->cover_path();
	}

	return getInvalidLocation().cover_path();
}

Location Location::getInvalidLocation()
{
	Location cl;

	cl.m->cover_path = ::Util::share_path("logo.png");
	cl.m->search_urls.clear();
	cl.m->search_term = "";
	cl.m->valid = false;
	cl.m->identifier = "Invalid location";

	return cl;
}

bool Location::isInvalidLocation(const QString& cover_path)
{
	QString path1 = ::Util::File::clean_filename(cover_path);
	QString path2 = getInvalidLocation().cover_path();

	return (path1 == path2);
}


Location Location::get_cover_location(const QString& album_name, const QString& artist_name)
{
	QString cover_token = Cover::Util::calc_cover_token(artist_name, album_name);
	QString cover_path = get_cover_directory( cover_token + ".jpg" );

	Location ret;
	Fetcher::Manager* cfm = Fetcher::Manager::instance();

	ret.m->cover_path = cover_path;
	ret.m->search_term = artist_name + " " + album_name;
	ret.m->search_urls.clear();
	ret.m->search_urls << cfm->album_addresses(artist_name, album_name);
	ret.m->valid = true;

	ret.m->identifier = "CL:By album: " + album_name + " by " + artist_name;

	return ret;
}

Location Location::get_cover_location(const QString& album_name, const QStringList& artists)
{
	QString major_artist = ArtistList::get_major_artist(artists);
	return get_cover_location(album_name, major_artist);
}



// TODO: Clean me up
// TODO: Check for albumID
// TODO: Check for dbid
// TODO: Make this class nicer: e.g. valid(), isInvalidLocation()
Location Location::get_cover_location(const Album& album)
{
	int n_artists = album.artists().size();

	Location cl;

	if( n_artists > 1 ) {
		cl = Location::get_cover_location(album.name(), album.artists());
	}

	else if( n_artists == 1 ) {
		cl = Location::get_cover_location(album.name(), album.artists().first());
	}

	else {
		cl = Location::get_cover_location(album.name(), "");
	}

	if(!album.cover_download_url().isEmpty()){
		cl.m->search_urls.clear();
		cl.m->search_urls << album.cover_download_url();
	}


	if(!cl.valid() || !QFile::exists(cl.cover_path()))
	{
		// TODO: Only look in the database once for covers
		// If we check covers in the database every time,
		// people are not amused
		//return cl;

		DB::Connector* db = DB::Connector::instance();
		DB::LibraryDatabase* lib_db = db->library_db(-1, 0);

		MetaDataList v_md;
		lib_db->getAllTracksByAlbum(album.id, v_md);

		for(const MetaData& md : v_md){
			cl.m->local_paths = Cover::LocalSearcher::cover_paths_from_filename(md.filepath());
			if(!cl.m->local_paths.isEmpty()){
				break;
			}
		}

		Location tmpcl;
		if(!Location::isInvalidLocation(cl.preferred_path())){
			QImage img(cl.preferred_path());

			if( n_artists > 1){
				tmpcl = Location::get_cover_location(album.name(), album.artists());
			}

			else if( n_artists == 1){
				tmpcl = Location::get_cover_location(album.name(), album.artists().first());
			}

			else{
				tmpcl = Location::get_cover_location(album.name(), "");
			}

			img.save(tmpcl.cover_path());
		}
	}

	cl.m->search_term = album.name() + " " + ArtistList::get_major_artist(album.artists());

	return cl;
}

Location Location::get_cover_location(const Artist& artist)
{
	Location cl = Location::get_cover_location(artist.name());

	if(!artist.cover_download_url().isEmpty()){
		cl.m->search_urls.clear();
		cl.m->search_urls << artist.cover_download_url();
	}

	cl.m->search_term = artist.name();
	cl.m->identifier = "CL:By artist: " + artist.name();

	return cl;
}


Location Location::get_cover_location(const QString& artist)
{
	if(artist.isEmpty()) return getInvalidLocation();

	QString cover_token = QString("artist_") + Cover::Util::calc_cover_token(artist, "");
	QString cover_path = get_cover_directory(cover_token + ".jpg");

	Location ret;
	Fetcher::Manager* cfm = Fetcher::Manager::instance();

	ret.m->cover_path = cover_path;
	ret.m->search_urls = cfm->artist_addresses(artist);
	ret.m->search_term = artist;
	ret.m->valid = true;
	ret.m->identifier = "CL:By artist name: " + artist;

	return ret;
}




Location Get_cover_location(int album_id, uint8_t db_id)
{
	if(album_id < 0) {
		return Location::getInvalidLocation();
	}

	Album album;
	MetaDataList v_md;

	DB::Connector* db = DB::Connector::instance();
	// TODO: Why?
	DB::LibraryDatabase* lib_db = db->library_db(-1, db_id);
	if(!lib_db){
		return Location();
	}

	bool success = lib_db->getAlbumByID(album_id, album, true);

	if(!success) {
		return Location::getInvalidLocation();
	}

	Location cl = Location::get_cover_location(album);

	lib_db->getAllTracksByAlbum(album_id, v_md);
	for(const MetaData& md : v_md)
	{
		QStringList local_paths = Cover::LocalSearcher::cover_paths_from_filename(md.filepath());
		for(const QString& local_path : local_paths){
			cl.add_local_path(local_path);
		}

		if(!cl.local_paths().isEmpty()){
			break;
		}
	}

	return cl;
}

Location Location::get_cover_location(const MetaData& md)
{
	Location cl;


	if(md.album_id >= 0){
		cl = Get_cover_location(md.album_id, md.db_id());
	}

	if(!cl.valid()){
		cl = get_cover_location(md.album(), md.artist());
	}

	if(!md.cover_download_url().isEmpty())
	{
		QString extension = ::Util::File::get_file_extension(md.cover_download_url());

		QString cover_token = Cover::Util::calc_cover_token(md.artist(), md.album());
		QString cover_path = get_cover_directory(cover_token + "." + extension);

		cl = get_cover_location(QUrl(md.cover_download_url()), cover_path);
	}

	if(cl.m->search_urls.isEmpty()){
		cl.m->search_urls = QStringList(md.cover_download_url());
	}

	cl.m->local_paths << Cover::LocalSearcher::cover_paths_from_filename(md.filepath());
	cl.m->identifier = "CL:By metadata: " + md.album() + " by " + md.artist();

	return cl;
}


Location Location::get_cover_location(const QUrl& url, const QString& target_path)
{
	Location cl;
	cl.m->cover_path = target_path;
	cl.m->search_urls = QStringList(url.toString());
	cl.m->valid = true;
	cl.m->identifier = "CL:By direct download url: " + url.toString();

	return cl;
}


bool Location::valid() const
{
	return m->valid;
}

QStringList Location::local_paths() const
{
	return m->local_paths;
}

void Location::add_local_path(const QString& path)
{
	m->local_paths << path;
}

QString Location::local_path(int idx) const
{
	if(!between(idx, m->local_paths)){
		return QString();
	}

	return m->local_paths[idx];
}

QString Location::cover_path() const
{
	return m->cover_path;
}

QString Location::identifer() const
{
	return m->identifier;
}

QStringList Location::search_urls() const
{
	return m->search_urls;
}

void Location::remove_first_search_url()
{
	if(!m->search_urls.isEmpty()){
		m->search_urls.removeFirst();
	}
}

bool Location::has_search_urls() const
{
	return !(m->search_urls.isEmpty());
}

QString Location::search_term() const
{
	return m->search_term;
}

void Location::set_search_term(const QString& search_term)
{
	Fetcher::Manager* cfm = Fetcher::Manager::instance();

	m->search_term = search_term;
	m->search_urls = cfm->search_addresses(search_term);
}

QString Location::to_string() const
{
	return "Cover Location: Valid? " + QString::number(m->valid) +
			" - Cover path: " + m->cover_path + " - " + m->local_paths.join(",");
}
