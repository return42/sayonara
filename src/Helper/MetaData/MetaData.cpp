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

#define MD_INIT(x) x(other.x)
#define MD_MOVE(x) x(std::move(other.x))
#define MD_ASSIGN(x) x = other.x
#define MD_CMP(x) (x == other.x)

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
	qint32 		album_artist_id;
	RadioMode	radio_mode;

	Private() :
		album_artist_id(-1),
		radio_mode(RadioMode::Off)
	{}

	Private(const Private& other) :
		MD_INIT(album_artist),
		MD_INIT(filepath),
		MD_INIT(album_artist_id),
		MD_INIT(radio_mode)
	{}

	Private(Private&& other) :
		MD_MOVE(album_artist),
		MD_MOVE(filepath),
		MD_MOVE(album_artist_id),
		MD_MOVE(radio_mode)
	{}

	Private& operator=(const Private& other)
	{
		MD_ASSIGN(album_artist);
		MD_ASSIGN(filepath);
		MD_ASSIGN(album_artist_id);
		MD_ASSIGN(radio_mode);

		return *this;
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
	MD_INIT(title),
	MD_INIT(artist),
	MD_INIT(album),
	MD_INIT(genres),
	MD_INIT(length_ms),
	MD_INIT(filesize),
	MD_INIT(id),
	MD_INIT(artist_id),
	MD_INIT(album_id),
	MD_INIT(bitrate),
	MD_INIT(track_num),
	MD_INIT(year),
	MD_INIT(played),
	MD_INIT(is_extern),
	MD_INIT(pl_playing),
	MD_INIT(is_disabled),
	MD_INIT(rating),
	MD_INIT(discnumber),
	MD_INIT(n_discs),
	MD_INIT(library_id)

{
	_m = Pimpl::make<Private>(*(other._m));
}


MetaData::MetaData(MetaData&& other) :
	LibraryItem(other),
	MD_MOVE(title),
	MD_MOVE(artist),
	MD_MOVE(album),
	MD_MOVE(genres),
	MD_MOVE(length_ms),
	MD_MOVE(filesize),
	MD_MOVE(id),
	MD_MOVE(artist_id),
	MD_MOVE(album_id),
	MD_MOVE(bitrate),
	MD_MOVE(track_num),
	MD_MOVE(year),
	MD_MOVE(played),
	MD_MOVE(is_extern),
	MD_MOVE(pl_playing),
	MD_MOVE(is_disabled),
	MD_MOVE(rating),
	MD_MOVE(discnumber),
	MD_MOVE(n_discs),
	MD_MOVE(library_id)
{
	_m = Pimpl::make<Private>(*(other._m));
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

	MD_ASSIGN(title);
	MD_ASSIGN(artist);
	MD_ASSIGN(album);
	MD_ASSIGN(genres);
	MD_ASSIGN(length_ms);
	MD_ASSIGN(filesize);
	MD_ASSIGN(id);
	MD_ASSIGN(artist_id);
	MD_ASSIGN(album_id);
	MD_ASSIGN(bitrate);
	MD_ASSIGN(track_num);
	MD_ASSIGN(year);
	MD_ASSIGN(played);
	MD_ASSIGN(is_extern);
	MD_ASSIGN(pl_playing);
	MD_ASSIGN(is_disabled);
	MD_ASSIGN(rating);
	MD_ASSIGN(discnumber);
	MD_ASSIGN(n_discs);
	MD_ASSIGN(library_id);

	*(_m) = *(other._m);

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


qint32 MetaData::album_artist_id() const
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

void MetaData::set_album_artist(const QString& album_artist, qint32 id)
{
	_m->album_artist = album_artist;
	_m->album_artist_id = id;
}

void MetaData::set_album_artist_id(qint32 id)
{
	_m->album_artist_id = id;
}

bool MetaData::has_album_artist() const
{
	return (!_m->album_artist.isEmpty() && _m->album_artist_id >= 0);
}


