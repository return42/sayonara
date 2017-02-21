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
 * @ingoup MetaDataHelper
 */
class MetaData :
		public LibraryItem
{
private:
	QString		_filepath;
	RadioMode	_radio_mode;

	qint32		_album_artist_id;
	QString		_album_artist;

public:
	qint32 id;
	QString title;

	qint32 artist_id;
	QString artist;

	qint32 album_id;
	QString album;          // if radio station: name of radio station

	SP::Set<Genre> genres;
	quint8 rating;
	quint64 length_ms;
	quint16 year;

	quint16 track_num;
	quint32 bitrate;
	quint64 filesize;

	quint8 discnumber;
	quint8 n_discs;

	bool played;
	bool is_extern;
	bool pl_playing;
	bool is_disabled;

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
