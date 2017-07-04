/* Album.cpp */

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

#include "Helper/MetaData/Album.h"

#include <QVariant>

#define ALBUM_DO_COPY \
	name = other.name; \
	id = other.id; \
	num_songs = other.num_songs; \
	length_sec = other.length_sec; \
	year = other.year; \
	artists = other.artists; \
	discnumbers = other.discnumbers; \
	n_discs = other.n_discs; \
	rating = other.rating; \
	is_sampler = other.is_sampler; \
	_album_artists = other._album_artists;

Album::Album() : LibraryItem()
{
    name = "";
    id = -1;
    num_songs = 0;
    length_sec = 0;
    year = 0;
    n_discs = 1;
    is_sampler = false;
    rating = 0;
}

Album::Album(const Album& other) :
	LibraryItem(other)
{
	ALBUM_DO_COPY
}

Album::Album(Album&& other) :
	LibraryItem(other)
{
	ALBUM_DO_COPY
}

Album& Album::operator=(const Album& other)
{
	LibraryItem::operator =(other);
	ALBUM_DO_COPY
	return *this;
}


Album::~Album() {}


bool Album::has_album_artists() const
{
	return (_album_artists.size() > 0);
}

QStringList Album::album_artists() const
{
	return _album_artists;
}

void Album::set_album_artists(const QStringList& artists)
{
	_album_artists.clear();
	for(const QString& artist : artists){
		if(!artist.trimmed().isEmpty()){
			_album_artists << artist;
		}
	}
}


QVariant Album::toVariant(const Album& album)
{
	QVariant var; 
	var.setValue(album);
	return var;
}


bool Album::fromVariant(const QVariant& v, Album& album) {
	if( !v.canConvert<Album>() ) return false;
	album =	v.value<Album>();
	return true;
}


bool AlbumList::contains(int32_t album_id) const
{
	for(auto it=this->begin(); it!=this->end(); it++){
		if(it->id == album_id){
			return true;
		}
	}

	return false;
}
