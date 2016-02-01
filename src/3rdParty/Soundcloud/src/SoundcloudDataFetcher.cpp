/* SoundcloudDataFetcher.cpp */

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



#include "SoundcloudDataFetcher.h"
#include "SoundcloudWebAccess.h"
#include "SoundcloudJsonParser.h"

#include "Helper/WebAccess/AsyncWebAccess.h"


SoundcloudDataFetcher::SoundcloudDataFetcher(QObject* parent) :
	QObject(parent)
{
	_artist_id = -1;
}

SoundcloudDataFetcher::~SoundcloudDataFetcher()
{

}


void SoundcloudDataFetcher::search_artists(const QString& artist_name){

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::artists_fetched);
	awa->run( SoundcloudWebAccess::create_dl_get_artist(artist_name));
}

void SoundcloudDataFetcher::get_artist(int artist_id){
	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::artists_fetched);

	awa->run( SoundcloudWebAccess::create_dl_get_artist(artist_id) );
}


void SoundcloudDataFetcher::get_playlist(int playlist_id){
	Q_UNUSED(playlist_id)
	// Not implemented
}


void SoundcloudDataFetcher::get_track(int track_id){

	AsyncWebAccess* awa = new AsyncWebAccess(this);

	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::tracks_fetched);

	awa->run( SoundcloudWebAccess::create_dl_get_track(track_id) );
}


void SoundcloudDataFetcher::get_playlists_by_artist(int artist_id){
	return get_tracks_by_artist(artist_id);
}


void SoundcloudDataFetcher::get_tracks_by_artist(int artist_id){

	_album_tracks.clear();
	_artist_id = artist_id;

	AsyncWebAccess* awa = new AsyncWebAccess(this);
	connect(awa, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::playlist_tracks_fetched);

	awa->run( SoundcloudWebAccess::create_dl_get_playlists(artist_id) );
}


void SoundcloudDataFetcher::artists_fetched(bool success){

	ArtistList artists;
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);
	parser.parse_artists(artists);

	emit sig_artists_fetched(artists);
	awa->deleteLater();
}


void SoundcloudDataFetcher::playlist_tracks_fetched(bool success){

	MetaDataList v_md;
	AlbumList playlists;
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	AsyncWebAccess* awa2 = nullptr;
	int artist_id = -1;

	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);
	parser.parse_playlists(playlists, v_md);

	_album_tracks = v_md;
	emit sig_playlists_fetched(playlists);

	if(!v_md.isEmpty()){
		artist_id = v_md[0].artist_id;
	}

	else {
		artist_id = _artist_id;
	}

	if(artist_id == -1){
		emit sig_tracks_fetched(v_md);
		awa->deleteLater();
		return;
	}

	awa2 = new AsyncWebAccess(this);

	connect(awa2, &AsyncWebAccess::sig_finished,
			this, &SoundcloudDataFetcher::tracks_fetched);

	awa2->run( SoundcloudWebAccess::create_dl_get_tracks(artist_id) );

	awa->deleteLater();
}


void SoundcloudDataFetcher::tracks_fetched(bool success){

	MetaDataList v_md;
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!success){
		awa->deleteLater();
		return;
	}

	QByteArray data = awa->get_data();
	SoundcloudJsonParser parser(data);

	parser.parse_tracks(v_md);


	// v_md are tracks fetched without
	// album connection
	for(MetaData& md : v_md){
		for(const MetaData& album_md : _album_tracks){
			if(album_md.id == md.id){
				md.album_id = album_md.album_id;
				md.album = album_md.album;
				md.track_num = album_md.track_num;
				break;
			}
		}

		if(md.album.isEmpty() || md.album_id == -1){
			md.album = "None";
			md.album_id = 0;
		}
	}

	emit sig_tracks_fetched(v_md);

	awa->deleteLater();
}
