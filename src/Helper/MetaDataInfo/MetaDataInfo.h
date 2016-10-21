/* MetaDataInfo.h */

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

#ifndef METADATAINFO_H
#define METADATAINFO_H

#include "Helper/Set.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Helper/typedefs.h"

#include "Components/Covers/CoverLocation.h"

#include <QMap>
#include <QObject>
#include <QStringList>

class MetaDataList;
class LibraryDatabase;

/**
 * @brief The InfoStrings enum
 * @ingroup MetaDataHelper
 */
enum class InfoStrings : quint8
{
	nTracks=0,		// set by MetaDataInfo
	nAlbums,		// set by ArtistInfo, AlbumInfo
	nArtists,		// set by ArtistInfo, AlbumInfo
	Filesize,		// set by MetaDataInfo
	PlayingTime,	// set by MetaDataInfo
	Year,			// set by MetaDataInfo
	Sampler,		// set by AlbumInfo
	Bitrate,		// set by MetaDataInfo
	Genre			// set by MetaDataInfo
};



/**
 * @brief The MetaDataInfo class
 * @ingroup MetaDataHelper
 */
class MetaDataInfo : 
	public QObject, 
	protected SayonaraClass
{

private:
	void set_cover_location(const MetaDataList& lst);
	void set_subheader(quint16 tracknum);
	void set_header(const MetaDataList& lst);

	QString get_info_string(InfoStrings idx) const;

protected:

	QString						_header;
	QString						_subheader;
	QMap<InfoStrings, QString>	_info;
	QMap<QString, QString>		_additional_info;
	QStringList					_paths;
	CoverLocation				_cover_location;

	SP::Set<QString>			_albums;
	SP::Set<QString>			_artists;
	SP::Set<AlbumID>			_album_ids;
	SP::Set<ArtistID>			_artist_ids;

	LibraryDatabase*			_db=nullptr;

	QString calc_tracknum_str( quint16 tracknum );
	QString calc_artist_str();
	QString calc_album_str();

	virtual void set_cover_location();
	virtual void set_subheader();
	virtual void set_header();

	void insert_playing_time(quint64 ms);
	void insert_genre(const QStringList& lst);
	void insert_filesize(quint64 filesize);

	void insert_interval(InfoStrings key, int min, int max);
	void insert_number(InfoStrings key, int number);
	

	public:

	explicit MetaDataInfo(const MetaDataList& v_md);
	virtual ~MetaDataInfo();

	QString get_header() const;
	QString get_subheader() const;
	QString get_info_as_string() const;
	virtual QString get_additional_info_as_string() const;
	QString get_paths_as_string() const;
	CoverLocation get_cover_location() const;
};


#endif // METADATAINFO_H
