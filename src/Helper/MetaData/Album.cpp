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

Album::Album() :
	LibraryItem(),
	n_discs(1),
	rating(0),
	is_sampler(false),
	id(-1),
	length_sec(0),
	num_songs(0),
	year(0) 
{}

Album::Album(const Album& other) :
	LibraryItem(other),
	CASSIGN(n_discs),
	CASSIGN(rating),
	CASSIGN(is_sampler),
	CASSIGN(id),
	CASSIGN(length_sec),
	CASSIGN(num_songs),
	CASSIGN(year)
{}

Album::Album(Album&& other) :
	LibraryItem(std::move(other))
	CMOVE(n_discs),
	CMOVE(rating),
	CMOVE(is_sampler),
	CMOVE(id),
	CMOVE(length_sec),
	CMOVE(num_songs),
	CMOVE(year)
{}

Album& Album::operator=(const Album& other)
{
	LibraryItem::operator =(other);

	ASSIGN(n_discs);
	ASSIGN(rating);
	ASSIGN(is_sampler);
	ASSIGN(id);
	ASSIGN(length_sec);
	ASSIGN(num_songs);
	ASSIGN(year;

	return *this;
}

Album& Album::operator=(Album&& other)
{
	LibraryItem::operator = (std::move(other));

	MOVE(n_discs);
	MOVE(rating);
	MOVE(is_sampler);
	MOVE(id);
	MOVE(length_sec);
	MOVE(num_songs);
	MOVE(year);

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
