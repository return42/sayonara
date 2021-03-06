/* AbstractLibrary.h */

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

#ifndef ABSTRACTLIBRARY_H
#define ABSTRACTLIBRARY_H


#include "Utils/Library/LibraryNamespaces.h"
#include "Utils/Library/Filter.h"
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"
#include "Utils/Set.h"

#include "Utils/MetaData/Artist.h"
#include "Utils/MetaData/Album.h"
#include "Utils/MetaData/MetaDataList.h"

#include "Utils/Library/Sorting.h"

#include <QFile>

#define prepare_tracks_for_playlist_files static_cast<void (AbstractLibrary::*) (const QStringList&)>(&AbstractLibrary::psl_prepare_tracks_for_playlist)
#define prepare_tracks_for_playlist_idxs static_cast<void (AbstractLibrary::*) (const IdxList&)>(&AbstractLibrary::psl_prepare_tracks_for_playlist)

namespace Tagging
{
	class Editor;
}

class AbstractLibrary :
		public QObject,
		public SayonaraClass
{
	Q_OBJECT
	PIMPL(AbstractLibrary)

public:

	explicit AbstractLibrary(QObject *parent=nullptr);
	virtual ~AbstractLibrary();

	Library::Sortings   sortorder() const;
	Library::Filter		filter() const;
	// calls fetch_by_filter and emits
	void change_filter(Library::Filter, bool force=false);

	const MetaDataList& tracks() const;
	const AlbumList& albums() const;
	const ArtistList& artists() const;
	const MetaDataList& current_tracks() const;

	const SP::Set<TrackID>& selected_tracks() const;
	const SP::Set<AlbumId>& selected_albums() const;
	const SP::Set<ArtistId>& selected_artists() const;

signals:
	void sig_track_mime_data_available();
	void sig_all_tracks_loaded ();
	void sig_all_albums_loaded();
	void sig_all_artists_loaded();

	void sig_reloading_library(const QString& message, int progress);
	void sig_reloading_library_finished();

	void sig_delete_answer(QString);
	void sig_import_dialog_requested();


public slots:

	virtual void load();
	bool is_loaded() const;

	virtual void reload_library(bool clear_first, Library::ReloadQuality quality)=0;

	/* Clears all filters and searchstrings and fetches everything again */
	virtual void refetch();


	/* refetches everything from database as it is, keeping selected elements,
	   the user won't recognize anything at all */
	virtual void refresh();


	/* selection changed */
	virtual void selected_artists_changed(const IndexSet& indexes);
	virtual void selected_albums_changed(const IndexSet& indexes);
	virtual void selected_tracks_changed(const IndexSet& indexes);

	// Those two functions are identical (1) calls (2)
	virtual void prepare_current_tracks_for_playlist(bool new_playlist);
	virtual void prepare_fetched_tracks_for_playlist(bool new_playlist);
	void prepare_tracks_for_playlist(const QStringList& file_paths, bool new_playlist);


	/* append tracks after current played track in playlist */
	virtual void play_next_fetched_tracks();
	virtual void play_next_current_tracks();


	/* append tracks after last track in playlist */
	virtual void append_fetched_tracks();
	virtual void append_current_tracks();

	/* triggered by tagedit */
	virtual void metadata_id3_changed(const MetaDataList&, const MetaDataList&);

	/* a searchfilter has been entered, nothing is emitted */
	virtual void fetch_by_filter(Library::Filter filter, bool force);
	virtual void fetch_tracks_by_paths(const QStringList& paths);

	virtual void delete_tracks(const MetaDataList& v_md, Library::TrackDeletionMode mode)=0;
	virtual void delete_tracks_by_idx(const IndexSet& indexes, Library::TrackDeletionMode mode);

	virtual void delete_fetched_tracks(Library::TrackDeletionMode mode);
	virtual void delete_current_tracks(Library::TrackDeletionMode mode);
	virtual void delete_all_tracks();

	virtual void insert_tracks(const MetaDataList& v_md);
	virtual void import_files(const QStringList& files);

	/* write new rating to database */
	virtual void change_track_rating(int idx, int rating);
	virtual void change_album_rating(int idx, int rating);

	virtual void change_track_sortorder(Library::SortOrder s);
	virtual void change_album_sortorder(Library::SortOrder s);
	virtual void change_artist_sortorder(Library::SortOrder s);

	virtual void add_genre(const IdSet ids, const Genre& genre);
	virtual void delete_genre(const Genre& genre);
	virtual void rename_genre(const Genre& genre, const Genre& new_genre);

	/* Check for current selected artist if out of date and
	 * fetch new data */
	virtual void refresh_artist()=0;
	virtual void refresh_albums()=0;
	virtual void refresh_tracks()=0;

protected:
	/* Emit 3 signals with shown artists, shown album, shown tracks */
	virtual void 		emit_stuff();

	virtual void		get_all_artists(ArtistList& artists)=0;
	virtual void		get_all_artists_by_searchstring(Library::Filter filter, ArtistList& artists)=0;

	virtual void		get_all_albums(AlbumList& albums)=0;
	virtual void		get_all_albums_by_artist(IdList artist_ids, AlbumList& albums, Library::Filter filter)=0;
	virtual void		get_all_albums_by_searchstring(Library::Filter filter, AlbumList& albums)=0;

	virtual void		get_all_tracks(MetaDataList& v_md)=0;
	virtual void		get_all_tracks(const QStringList& paths, MetaDataList& v_md)=0;
	virtual void		get_all_tracks_by_artist(IdList artist_ids, MetaDataList& v_md, Library::Filter filter)=0;
	virtual	void		get_all_tracks_by_album(IdList album_ids, MetaDataList& v_md, Library::Filter filter)=0;
	virtual void		get_all_tracks_by_searchstring(Library::Filter filter, MetaDataList& v_md)=0;

	virtual void		get_album_by_id(int album_id, Album& album)=0;
	virtual void		get_artist_by_id(int artist_id, Artist& artist)=0;

	virtual void		update_track(const MetaData& md)=0;
	virtual void		update_tracks(const MetaDataList& v_md);
	virtual void		update_album(const Album& album)=0;

	Tagging::Editor*      tag_edit();


	MetaDataList        _tracks;
	AlbumList			_albums;
	ArtistList			_artists;


private:

	void tag_edit_commit();
	void set_playlist_action_after_double_click();

	void change_track_selection(const IndexSet& indexes);
	void change_artist_selection(const IndexSet& indexes);
	void change_album_selection(const IndexSet& indexes);
};

#endif // ABSTRACTLIBRARY_H
