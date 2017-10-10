/* DatabaseTracks.h */

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

#ifndef DATABASETRACKS_H
#define DATABASETRACKS_H

#include "Database/DatabaseSearchMode.h"
#include "Utils/typedefs.h"
#include "Utils/Library/Sortorder.h"
#include "Utils/Pimpl.h"

#include <QString>

namespace Library {class Filter;}

class MetaData;
class MetaDataList;

class DatabaseTracks :
		private DatabaseSearchMode
{
	PIMPL(DatabaseTracks)

protected:
    void change_artistid_field(const QString& id, const QString& name);
	void change_track_lookup_field(const QString& track_lookup_field);

	void check_track_views(int8_t library_id);
	void check_track_view(int8_t library_id);
	void check_track_search_view(const QString& track_view, const QString& track_search_view);

protected:
	QString append_track_sort_string(QString querytext, Library::SortOrder sort);

public:
	DatabaseTracks(const QSqlDatabase& db, uint8_t db_id, int8_t _library_id);
	~DatabaseTracks();

	virtual bool db_fetch_tracks(SayonaraQuery& q, MetaDataList& result);

	virtual bool getAllTracksByAlbum(int album, MetaDataList& result);
	virtual bool getAllTracksByAlbum(int album, MetaDataList& result, const Library::Filter& filter, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc, int discnumber=-1);
	virtual bool getAllTracksByAlbum(IDList albums, MetaDataList& result);
	virtual bool getAllTracksByAlbum(IDList albums, MetaDataList& result, const Library::Filter& filter, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc);

	virtual bool getAllTracksByArtist(int artist, MetaDataList& result);
	virtual bool getAllTracksByArtist(int artist, MetaDataList& result, const Library::Filter& filter, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc);
	virtual bool getAllTracksByArtist(IDList artists, MetaDataList& result);
	virtual bool getAllTracksByArtist(IDList artists, MetaDataList& result, const Library::Filter& filter, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc);

	virtual bool getAllTracksBySearchString(const Library::Filter& filter, MetaDataList& result, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc);

	virtual bool insertTrackIntoDatabase (const MetaData& data,int artist_id, int album_id);
	virtual bool insertTrackIntoDatabase (const MetaData& data,int artist_id, int album_id, int album_artist_id);
	virtual bool updateTrack(const MetaData& data);
	virtual bool updateTracks(const MetaDataList& lst);

	virtual bool getAllTracks(MetaDataList& returndata, Library::SortOrder sortorder = Library::SortOrder::TrackArtistAsc);
	virtual MetaData getTrackById(int id);
	virtual MetaData getTrackByPath(const QString& path);
	virtual bool getMultipleTracksByPath(const QStringList& paths, MetaDataList& v_md);

	virtual bool deleteTrack(int id);
	virtual bool deleteTracks(const MetaDataList&);
	virtual bool deleteTracks(const IDList& ids);
	virtual bool deleteInvalidTracks(const QString& library_path=QString());

	virtual QString fetch_query_tracks() const;

	virtual QStringList getAllGenres();
	virtual void updateTrackCissearch();

	void deleteAllTracks();
};

#endif // DATABASETRACKS_H
