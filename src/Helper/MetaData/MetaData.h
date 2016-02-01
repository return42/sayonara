/* MetaData.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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
 *      Author: luke
 */

#ifndef METADATA_H_
#define METADATA_H_

#include "Helper/globals.h"
#include "Helper/MetaData/LibraryItem.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/MetaData/Album.h"
#include "Helper/Logger/Logger.h"
#include "Components/CoverLookup/CoverLocation.h"

#include <QPair>
#include <QMetaType>

#include <functional>

class AlbumInfo;
class ArtistInfo;
class MetaDataInfo;



enum class RadioMode : quint8
{

	Off = 0,
	Station,
	Soundcloud
};

class MetaData :
		public LibraryItem
{

private:
	QString _filepath;
	RadioMode _radio_mode;

public:
	qint32 id;
	qint32 album_id;
	qint32 artist_id;
	QString title;
	QString artist;
	QString album;          // if radio station: name of radio station
	QStringList genres;
	quint8 rating;
	quint64 length_ms;
	quint16 year;

	quint16 track_num;
	quint32 bitrate;
	quint64 filesize;
	QString comment;
	quint8 discnumber;
	quint8 n_discs;

	bool played;
	bool is_extern;
	bool pl_playing;

	bool is_disabled;

	MetaData ();
	MetaData (const MetaData& );
	MetaData ( MetaData&& );
	virtual ~MetaData();

	QUrl url() const;
	QString filepath() const;
	QString set_filepath(QString filepath);

	RadioMode radio_mode() const;

	MetaData& operator=(const MetaData& md);
	bool operator==(const MetaData& md) const;
	bool operator!=(const MetaData& md) const;
	bool is_equal(const MetaData& md) const;
	bool is_equal_deep(const MetaData& md) const;

	QString to_string() const;

	static QVariant toVariant(const MetaData& md);
	static bool fromVariant(const QVariant& v, MetaData& md);
};

Q_DECLARE_METATYPE(MetaData)



class MetaDataList : public QVector<MetaData> {

private:
	int _cur_played_track;

public:

	MetaDataList();
	MetaDataList(const MetaDataList&);
	MetaDataList(int n_elems);

	virtual ~MetaDataList();

	void set_cur_play_track(int idx);
	void set_cur_play_track_idx_only(int idx);
	int get_cur_play_track() const;


	virtual bool contains(const MetaData& md) const;
	MetaDataList& remove_track(int idx);
	MetaDataList& remove_tracks(IdxList rows);

	MetaDataList& move_tracks(const IdxList& rows, int tgt_idx);
	MetaDataList& insert_tracks(const MetaDataList& v_md, int tgt_idx);
	MetaDataList& randomize();

	MetaDataList extract_tracks(std::function<bool (const MetaData& md)> func) const;
	MetaDataList extract_tracks(const IdxList& idx_list) const;

	IdxList findTracks(int id) const;
	IdxList findTracks(const QString&) const;

	QStringList toStringList() const;

	MetaDataInfo* get_info() const ;
	ArtistInfo* get_artist_info() const ;
	AlbumInfo* get_album_info() const;


};


#endif /* METADATA_H_ */
