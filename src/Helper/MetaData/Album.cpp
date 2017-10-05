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
	id(-1),
	length_sec(0),
	num_songs(0),
	year(0),
	n_discs(1),
	rating(0),
	is_sampler(false)
{}

Album::Album(const Album& other) :
	LibraryItem(other),
	CASSIGN(_album_artists),
	CASSIGN(name),
	CASSIGN(artists),
	CASSIGN(discnumbers),
	CASSIGN(id),
	CASSIGN(length_sec),
	CASSIGN(num_songs),
	CASSIGN(year),
	CASSIGN(n_discs),
	CASSIGN(rating),
	CASSIGN(is_sampler)
{}

Album::Album(Album&& other) :
	LibraryItem(std::move(other)),
	CMOVE(_album_artists),
	CMOVE(name),
	CMOVE(artists),
	CMOVE(discnumbers),
	CMOVE(id),
	CMOVE(length_sec),
	CMOVE(num_songs),
	CMOVE(year),
	CMOVE(n_discs),
	CMOVE(rating),
	CMOVE(is_sampler)
{}

Album& Album::operator=(const Album& other)
{
	LibraryItem::operator =(other);

	ASSIGN(_album_artists);
	ASSIGN(name);
	ASSIGN(artists);
	ASSIGN(discnumbers);
	ASSIGN(id);
	ASSIGN(length_sec);
	ASSIGN(num_songs);
	ASSIGN(year);
	ASSIGN(n_discs);
	ASSIGN(rating);
	ASSIGN(is_sampler);

	return *this;
}

Album& Album::operator=(Album&& other)
{
	LibraryItem::operator = (std::move(other));

	MOVE(_album_artists);
	MOVE(name);
	MOVE(artists);
	MOVE(discnumbers);
	MOVE(id);
	MOVE(length_sec);
	MOVE(num_songs);
	MOVE(year);
	MOVE(n_discs);
	MOVE(rating);
	MOVE(is_sampler);

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


bool AlbumList::contains(AlbumID album_id) const
{
	for(auto it=this->begin(); it!=this->end(); it++){
		if(it->id == album_id){
			return true;
		}
	}

    return false;
}

int AlbumList::count() const
{
    return static_cast<int>(this->size());
}

AlbumList& AlbumList::operator <<(const Album &album)
{
    this->push_back(album);
    return *this;
}

Album AlbumList::first() const
{
    if(this->empty()){
        return Album();
    }

    return this->at(0);
}
