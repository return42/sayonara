/* MetaData.cpp */

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

#include "Helper/MetaData/Genre.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Logger/Logger.h"


#include <QDir>
#include <QUrl>
#include <QVariant>
#include <QStringList>

/*struct MDCounter
{
	int c=0;
	int m=0;
	void increase()
{
		c++;
		m=std::max(c, m);
		sp_log(Log::Debug, this) << "Num MD: " << c << " / " <<  m;
	}

	void decrease()
{
		c--;
		m=std::max(c, m);
		sp_log(Log::Debug, this) << "Num MD: " << c << " / " <<  m;
	}
};


static MDCounter mdc;*/

struct MetaData::Private
{
	QString		album_artist;
	QString		filepath;
	int32_t		album_artist_id;
	RadioMode	radio_mode;

	Private() :
		album_artist_id(-1),
		radio_mode(RadioMode::Off)
	{}

	Private(const Private& other) :
		CASSIGN(album_artist),
		CASSIGN(filepath),
		CASSIGN(album_artist_id),
		CASSIGN(radio_mode)
	{}

	Private(Private&& other) :
		CMOVE(album_artist),
		CMOVE(filepath),
		CMOVE(album_artist_id),
		CMOVE(radio_mode)
	{}

	Private& operator=(const Private& other)
	{
		ASSIGN(album_artist);
		ASSIGN(filepath);
		ASSIGN(album_artist_id);
		ASSIGN(radio_mode);

		return *this;
	}

	Private&& operator=(Private&& other)
	{
		MOVE(album_artist);
		MOVE(filepath);
		MOVE(album_artist_id);
		MOVE(radio_mode);
	}

	bool is_equal(const Private& other) const
	{
		return(
			MD_CMP(album_artist) &&
			MD_CMP(filepath) &&
			MD_CMP(album_artist_id) &&
			MD_CMP(radio_mode)
		);
	}
};

MetaData::MetaData() :
	LibraryItem(),
	length_ms(0),
	filesize(0),

	id(-1),
	artist_id(-1),
	album_id(-1),

	bitrate(0),
	track_num(0),
	year(0),

	played(false),
	is_extern(false),
	pl_playing(false),
	is_disabled(false),

	rating(0),
	discnumber(0),
	n_discs(0),

	library_id(-1)
{
	_m = Pimpl::make<Private>();
}

MetaData::MetaData(const MetaData& other) :
	LibraryItem(other),
	CASSIGN(title),
	CASSIGN(artist),
	CASSIGN(album),
	CASSIGN(genres),
	CASSIGN(length_ms),
	CASSIGN(filesize),
	CASSIGN(id),
	CASSIGN(artist_id),
	CASSIGN(album_id),
	CASSIGN(bitrate),
	CASSIGN(track_num),
	CASSIGN(year),
	CASSIGN(played),
	CASSIGN(is_extern),
	CASSIGN(pl_playing),
	CASSIGN(is_disabled),
	CASSIGN(rating),
	CASSIGN(discnumber),
	CASSIGN(n_discs),
	CASSIGN(library_id)

{
	_m = Pimpl::make<Private>(*(other._m));
}


MetaData::MetaData(MetaData&& other) :
	LibraryItem(other),
	CMOVE(title),
	CMOVE(artist),
	CMOVE(album),
	CMOVE(genres),
	CMOVE(length_ms),
	CMOVE(filesize),
	CMOVE(id),
	CMOVE(artist_id),
	CMOVE(album_id),
	CMOVE(bitrate),
	CMOVE(track_num),
	CMOVE(year),
	CMOVE(played),
	CMOVE(is_extern),
	CMOVE(pl_playing),
	CMOVE(is_disabled),
	CMOVE(rating),
	CMOVE(discnumber),
	CMOVE(n_discs),
	CMOVE(library_id)
{
	
	_m = Pimpl::make<Private>(
		std::move(*(other._m))
	);
}

MetaData::MetaData(const QString& path) :
	MetaData()
{
//	mdc.increase();
	this->set_filepath(path);
}

MetaData::~MetaData()
{
//	mdc.decrease();
}

QString MetaData::to_string() const
{
	QStringList lst;
	lst << title;
	lst << "by " << artist << " (" << album_artist() << ")";
	lst << "on " << album;
	lst << "Rating: " << QString::number(rating);
	lst << "Disc: " << QString::number(discnumber);
	lst << "Filepath: " << filepath();

	return lst.join(" - ");
}

QVariant MetaData::toVariant(const MetaData& md) 
{
	QVariant v;

	v.setValue<MetaData>(md);

	return v;
}

bool MetaData::fromVariant(const QVariant& v, MetaData& md) 
{
	if(!v.canConvert<MetaData>() ) {
		return false;
	}

	md = v.value<MetaData>() ;
	return true;
}

MetaData& MetaData::operator=(const MetaData& other)
{
	LibraryItem::operator=(other);
	
	(*_m) = *(other._m);

	ASSIGN(title);
	ASSIGN(artist);
	ASSIGN(album);
	ASSIGN(genres);
	ASSIGN(length_ms);
	ASSIGN(filesize);
	ASSIGN(id);
	ASSIGN(artist_id);
	ASSIGN(album_id);
	ASSIGN(bitrate);
	ASSIGN(track_num);
	ASSIGN(year);
	ASSIGN(played);
	ASSIGN(is_extern);
	ASSIGN(pl_playing);
	ASSIGN(is_disabled);
	ASSIGN(rating);
	ASSIGN(discnumber);
	ASSIGN(n_discs);
	ASSIGN(library_id);


	return *this;
}

MetaData& MetaData::operator=(MetaData&& other)
{
	LibraryItem::operator=(std::move(other));
	
	(*_m) = std::move(*(other._m));

	MOVE(title);
	MOVE(artist);
	MOVE(album);
	MOVE(genres);
	MOVE(length_ms);
	MOVE(filesize);
	MOVE(id);
	MOVE(artist_id);
	MOVE(album_id);
	MOVE(bitrate);
	MOVE(track_num);
	MOVE(year);
	MOVE(played);
	MOVE(is_extern);
	MOVE(pl_playing);
	MOVE(is_disabled);
	MOVE(rating);
	MOVE(discnumber);
	MOVE(n_discs);
	MOVE(library_id);

	return *this;
}


bool MetaData::operator==(const MetaData& md) const
{
	return this->is_equal(md);
}


bool MetaData::operator!=(const MetaData& md) const
{
	return !(this->is_equal(md));
}


bool MetaData::is_equal(const MetaData& md) const
{
	QDir first_path(_m->filepath);
	QDir other_path(md.filepath());

	QString s_first_path = first_path.absolutePath();
	QString s_other_path = other_path.absolutePath();

#ifdef Q_OS_UNIX
	return (s_first_path.compare(s_other_path) == 0);
#else
	return (s_first_path.compare(s_other_path, Qt::CaseInsensitive) == 0);
#endif

}

bool MetaData::is_equal_deep(const MetaData& other) const
{
	return 
	(
			MD_CMP(title) &&
			MD_CMP(artist) &&
			MD_CMP(album) &&
			MD_CMP(genres) &&
			MD_CMP(length_ms) &&
			MD_CMP(filesize) &&
			MD_CMP(id) &&
			MD_CMP(artist_id) &&
			MD_CMP(album_id) &&
			MD_CMP(bitrate) &&
			MD_CMP(track_num) &&
			MD_CMP(year) &&
			MD_CMP(played) &&
			MD_CMP(is_extern) &&
			MD_CMP(pl_playing) &&
			MD_CMP(is_disabled) &&
			MD_CMP(rating) &&
			MD_CMP(discnumber) &&
			MD_CMP(n_discs) &&
			MD_CMP(library_id) &&
			_m->is_equal(*(other._m))
	);
}

bool MetaData::has_genre(const Genre& genre) const
{
	for(const Genre& g : genres){
		if(g == genre){
			return true;
		}
	}

	return false;
}

bool MetaData::remove_genre(const Genre& genre)
{
	bool has_genre = false;
	auto it = genres.find(genre);
	if(it != genres.end()){
		has_genre = true;
		genres.erase(it);
	}

	return has_genre;
}

bool MetaData::add_genre(const Genre& genre)
{
	if(has_genre(genre)){
		return false;
	}

	genres << genre;
	return true;
}

void MetaData::set_genres(const QStringList& new_genres)
{
	this->genres.clear();
	for(const QString& genre : new_genres){
		this->genres << Genre(genre);
	}
}

QString MetaData::genres_to_string() const
{
	return genres_to_list().join(",");
}

QStringList MetaData::genres_to_list() const
{
	QStringList new_genres;
	for(const Genre& genre : genres){
		new_genres << genre.name();
	}

	return new_genres;
}

QString MetaData::filepath() const
{
	return _m->filepath;
}


QString MetaData::set_filepath(QString filepath)
{
	bool is_local_path = false;

#ifdef Q_OS_UNIX
	if(filepath.startsWith("/")){
		is_local_path = true;
	}
#else
	if(filepath.contains(":\\") || filepath.contains("\\\\")){
		is_local_path = true;
	}
#endif

	if(is_local_path){
		QDir dir(filepath);
		_m->filepath = dir.absolutePath();
		_m->radio_mode = RadioMode::Off;
	}

	else if(filepath.contains("soundcloud.com")){
		_m->filepath = filepath;
		_m->radio_mode = RadioMode::Soundcloud;
	}

	else{
		_m->filepath = filepath;
		_m->radio_mode = RadioMode::Station;
	}

	return _m->filepath;
}


RadioMode MetaData::radio_mode() const
{
	return _m->radio_mode;
}

bool MetaData::is_valid() const
{
	return (!filepath().isEmpty());
}


int32_t MetaData::album_artist_id() const
{
	if(_m->album_artist_id < 0){
		return artist_id;
	}

	return _m->album_artist_id;
}

QString MetaData::album_artist() const
{
	return _m->album_artist;
}

void MetaData::set_album_artist(const QString& album_artist, int32_t id)
{
	_m->album_artist = album_artist;
	_m->album_artist_id = id;
}

void MetaData::set_album_artist_id(int32_t id)
{
	_m->album_artist_id = id;
}

bool MetaData::has_album_artist() const
{
	return (!_m->album_artist.isEmpty() && _m->album_artist_id >= 0);
}


