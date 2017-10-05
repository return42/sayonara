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
#include "Helper/Set.h"

#include <QDir>
#include <QUrl>
#include <QVariant>
#include <QStringList>

//#define COUNT_MD
#ifdef COUNT_MD
    struct MDCounter
    {
        int c=0;
        int m=0;
        void increase()
        {
            c++;
            m++;
            sp_log(Log::Debug, this) << "Num MD: " << c << " / " <<  m;
        }

        void decrease()
        {
            c--;
            sp_log(Log::Debug, this) << "Num MD: " << c << " / " <<  m;
        }
    };

    static MDCounter mdc;
#endif

struct MetaData::Private
{
    SP::Set<Genre>  genres;
    QString         album_artist;
    QString         filepath;
    ArtistID        album_artist_id;
    RadioMode       radio_mode;

	Private() :
		album_artist_id(-1),
		radio_mode(RadioMode::Off)
	{}

	Private(const Private& other) :
        CASSIGN(genres),
		CASSIGN(album_artist),
		CASSIGN(filepath),
		CASSIGN(album_artist_id),
		CASSIGN(radio_mode)
	{}

	Private(Private&& other) :
        CMOVE(genres),
		CMOVE(album_artist),
		CMOVE(filepath),
		CMOVE(album_artist_id),
		CMOVE(radio_mode)
	{}

	Private& operator=(const Private& other)
	{
        ASSIGN(genres);
		ASSIGN(album_artist);
		ASSIGN(filepath);
		ASSIGN(album_artist_id);
		ASSIGN(radio_mode);

		return *this;
	}

	Private& operator=(Private&& other)
	{
        MOVE(genres);
		MOVE(album_artist);
		MOVE(filepath);
		MOVE(album_artist_id);
		MOVE(radio_mode);

		return *this;
	}

	bool is_equal(const Private& other) const
	{
		return(
            CMP(genres) &&
			CMP(album_artist) &&
			CMP(filepath) &&
			CMP(album_artist_id) &&
			CMP(radio_mode)
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

	discnumber(0),
	n_discs(0),

    library_id(-1),

    rating(0),
    played(false),
    is_extern(false),
    pl_playing(false),
    is_disabled(false)
{
	m = Pimpl::make<Private>();
#ifdef COUNT_MD
    mdc.increase();
#endif
}

MetaData::MetaData(const MetaData& other) :
	LibraryItem(other),
	CASSIGN(title),
	CASSIGN(artist),
	CASSIGN(album),
	CASSIGN(length_ms),
	CASSIGN(filesize),
	CASSIGN(id),
	CASSIGN(artist_id),
	CASSIGN(album_id),
	CASSIGN(bitrate),
	CASSIGN(track_num),
	CASSIGN(year),
	CASSIGN(discnumber),
	CASSIGN(n_discs),
    CASSIGN(library_id),
    CASSIGN(rating),
    CASSIGN(played),
    CASSIGN(is_extern),
    CASSIGN(pl_playing),
    CASSIGN(is_disabled)
{
	m = Pimpl::make<Private>(*(other.m));
#ifdef COUNT_MD
    mdc.increase();
#endif
}


MetaData::MetaData(MetaData&& other) :
	LibraryItem(other),
	CMOVE(title),
	CMOVE(artist),
	CMOVE(album),
	CMOVE(length_ms),
	CMOVE(filesize),
	CMOVE(id),
	CMOVE(artist_id),
	CMOVE(album_id),
	CMOVE(bitrate),
	CMOVE(track_num),
	CMOVE(year),
	CMOVE(discnumber),
	CMOVE(n_discs),
    CMOVE(library_id),
    CMOVE(rating),
    CMOVE(played),
    CMOVE(is_extern),
    CMOVE(pl_playing),
    CMOVE(is_disabled)
{
	m = Pimpl::make<Private>(
		std::move(*(other.m))
	);
#ifdef COUNT_MD
    mdc.increase();
#endif
}

MetaData::MetaData(const QString& path) :
	MetaData()
{
#ifdef COUNT_MD
    mdc.increase();
#endif
	this->set_filepath(path);
}

MetaData::~MetaData()
{
#ifdef COUNT_MD
    mdc.decrease();
#endif
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
	
	(*m) = *(other.m);

	ASSIGN(title);
	ASSIGN(artist);
	ASSIGN(album);
	ASSIGN(length_ms);
	ASSIGN(filesize);
	ASSIGN(id);
	ASSIGN(artist_id);
	ASSIGN(album_id);
	ASSIGN(bitrate);
	ASSIGN(track_num);
	ASSIGN(year);
	ASSIGN(discnumber);
	ASSIGN(n_discs);
	ASSIGN(library_id);
    ASSIGN(rating);
    ASSIGN(played);
    ASSIGN(is_extern);
    ASSIGN(pl_playing);
    ASSIGN(is_disabled);

	return *this;
}

MetaData& MetaData::operator=(MetaData&& other)
{
	LibraryItem::operator=(std::move(other));
	
	(*m) = std::move(*(other.m));

	MOVE(title);
	MOVE(artist);
	MOVE(album);
	MOVE(length_ms);
	MOVE(filesize);
	MOVE(id);
	MOVE(artist_id);
	MOVE(album_id);
	MOVE(bitrate);
	MOVE(track_num);
	MOVE(year);
	MOVE(discnumber);
	MOVE(n_discs);
	MOVE(library_id);
    MOVE(rating);
    MOVE(played);
    MOVE(is_extern);
    MOVE(pl_playing);
    MOVE(is_disabled);

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
	QDir first_path(m->filepath);
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
			CMP(title) &&
			CMP(artist) &&
			CMP(album) &&
			CMP(length_ms) &&
			CMP(filesize) &&
			CMP(id) &&
			CMP(artist_id) &&
			CMP(album_id) &&
			CMP(bitrate) &&
			CMP(track_num) &&
			CMP(year) &&
			CMP(played) &&
			CMP(is_extern) &&
			CMP(pl_playing) &&
			CMP(is_disabled) &&
			CMP(rating) &&
			CMP(discnumber) &&
			CMP(n_discs) &&
			CMP(library_id) &&
			m->is_equal(*(other.m))
	);
}


const SP::Set<Genre>& MetaData::genres() const
{
    return m->genres;
}

void MetaData::set_genres(const SP::Set<Genre>& genres)
{
    m->genres = genres;
}


bool MetaData::has_genre(const Genre& genre) const
{
    for(const Genre& g : m->genres){
		if(g == genre){
			return true;
		}
	}

	return false;
}

bool MetaData::remove_genre(const Genre& genre)
{
	bool has_genre = false;
    auto it = m->genres.find(genre);
    if(it != m->genres.end()){
		has_genre = true;
        m->genres.erase(it);
	}

	return has_genre;
}

bool MetaData::add_genre(const Genre& genre)
{
	if(has_genre(genre)){
		return false;
	}

    m->genres << genre;
	return true;
}

void MetaData::set_genres(const QStringList& new_genres)
{
    m->genres.clear();
	for(const QString& genre : new_genres){
        m->genres << Genre(genre);
    }
}


QString MetaData::genres_to_string() const
{
	return genres_to_list().join(",");
}

QStringList MetaData::genres_to_list() const
{
	QStringList new_genres;
    for(const Genre& genre : m->genres){
		new_genres << genre.name();
	}

	return new_genres;
}

QString MetaData::filepath() const
{
	return m->filepath;
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
		m->filepath = dir.absolutePath();
		m->radio_mode = RadioMode::Off;
	}

	else if(filepath.contains("soundcloud.com")){
		m->filepath = filepath;
		m->radio_mode = RadioMode::Soundcloud;
	}

	else{
		m->filepath = filepath;
		m->radio_mode = RadioMode::Station;
	}

	return m->filepath;
}


RadioMode MetaData::radio_mode() const
{
	return m->radio_mode;
}

bool MetaData::is_valid() const
{
	return (!filepath().isEmpty());
}


ArtistID MetaData::album_artist_id() const
{
	if(m->album_artist_id < 0){
		return artist_id;
	}

	return m->album_artist_id;
}

QString MetaData::album_artist() const
{
	return m->album_artist;
}

void MetaData::set_album_artist(const QString& album_artist, ArtistID id)
{
	m->album_artist = album_artist;
	m->album_artist_id = id;
}

void MetaData::set_album_artist_id(ArtistID id)
{
	m->album_artist_id = id;
}

bool MetaData::has_album_artist() const
{
	return (!m->album_artist.isEmpty() && m->album_artist_id >= 0);
}


