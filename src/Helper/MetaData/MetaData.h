/* MetaData.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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

/*
 * MetaData.h
 *
 *  Created on: Mar 10, 2011
 *      Author: Lucio Carreras
 */

#ifndef METADATA_H_
#define METADATA_H_

#include "Helper/MetaData/LibraryItem.h"
#include "Helper/Set.h"
#include "Helper/Pimpl.h"

#include <QMetaType>
#include <QStringList>


/**
 * @brief The RadioMode enum
 * @ingroup MetaDataHelper
 */
enum class RadioMode : quint8
{
	Off = 0,
	Station,
	Soundcloud
};

class Genre;

/**
 * @brief The MetaData class
 * @ingroup MetaDataHelper
 */
class MetaData :
		public LibraryItem
{
	PIMPL(MetaData)

public:
	QString title;
	QString artist;
	QString album;

	SP::Set<Genre> genres;
	quint64 length_ms;
	quint64 filesize;

  	qint32 id;
	qint32 artist_id;
	qint32 album_id;

	quint32 bitrate;
	quint16 track_num;
	quint16 year;

	bool played;
	bool is_extern;
	bool pl_playing;
	bool is_disabled;

	quint8 rating;
	quint8 discnumber;
	quint8 n_discs;
	qint8 library_id;

public:
	MetaData ();
	MetaData (const MetaData& );
	MetaData ( MetaData&& );

	explicit MetaData (const QString& path);
	~MetaData();

	QUrl url() const;
	QString filepath() const;
	QString set_filepath(QString filepath);

	qint32 album_artist_id() const;
	QString album_artist() const;
	bool has_album_artist() const;

	void set_album_artist(const QString& album_artist, qint32 id=-1);
	void set_album_artist_id(qint32 id);

	RadioMode radio_mode() const;

	MetaData& operator=(const MetaData& md);
	bool operator==(const MetaData& md) const;
	bool operator!=(const MetaData& md) const;
	bool is_equal(const MetaData& md) const;
	bool is_equal_deep(const MetaData& md) const;

	bool has_genre(const Genre& genre) const;
	bool remove_genre(const Genre& genre);
	bool add_genre(const Genre& genre);
	void set_genres(const QStringList& genres);
	QString genres_to_string() const;
	QStringList genres_to_list() const;

	QString to_string() const;

	static QVariant toVariant(const MetaData& md);
	static bool fromVariant(const QVariant& v, MetaData& md);
};

Q_DECLARE_METATYPE(MetaData)


#endif /* METADATA_H_ */
