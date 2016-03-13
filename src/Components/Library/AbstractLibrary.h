/* AbstractLibrary.h */

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



#ifndef ABSTRACTLIBRARY_H
#define ABSTRACTLIBRARY_H


#include "Helper/SayonaraClass.h"
#include "Helper/Tagging/Tagging.h"
#include <QFile>


#define prepare_tracks_for_playlist_files static_cast<void (AbstractLibrary::*) (const QStringList&)>(&AbstractLibrary::psl_prepare_tracks_for_playlist)
#define prepare_tracks_for_playlist_idxs static_cast<void (AbstractLibrary::*) (const IdxList&)>(&AbstractLibrary::psl_prepare_tracks_for_playlist)

class PlaylistHandler;
class EngineHandler;
class AbstractLibrary : public QObject, protected SayonaraClass
{
	Q_OBJECT

public:

	enum class TrackDeletionMode : quint8 {
		None=0,
		OnlyLibrary,
		AlsoFiles
	};

	explicit AbstractLibrary(QObject *parent=nullptr);
	virtual ~AbstractLibrary();




signals:
	void sig_track_mime_data_available(const MetaDataList&);
	void sig_all_tracks_loaded (const MetaDataList&);
	void sig_all_albums_loaded(const AlbumList&);
	void sig_all_artists_loaded(const ArtistList&);

	void sig_reloading_library(const QString& message, int progress);
	void sig_reloading_library_finished();

	void sig_delete_answer(QString);
	void sig_import_dialog_requested();



public slots:

	virtual void load();

	virtual void psl_reload_library(bool clear_first, Tagging::Quality quality)=0;

	/* Clears all filters and searchstrings and fetches everything again */
	virtual void refetch();


	/* refetches everything from database as it is, keeping selected elements,
	   the user won't recognize anything at all */
	virtual void refresh();



	/* selection changed */
	virtual void psl_selected_artists_changed(const IdxList& idx_lst);
	virtual void psl_selected_albums_changed(const IdxList& idx_lst);
	virtual void psl_selected_tracks_changed(const IdxList& idx_lst);


	// [albums|artist|track[s]] double clicked -> send tracks to playlist
	virtual void psl_prepare_album_for_playlist(int idx, bool new_playlist);
	virtual void psl_prepare_artist_for_playlist(int idx, bool new_playlist);


	// Those two functions are identical (1) calls (2)
	virtual void psl_prepare_track_for_playlist(int idx, bool new_playlist);
	virtual void psl_prepare_tracks_for_playlist(bool new_playlist);
	virtual void psl_prepare_tracks_for_playlist(const IdxList& idx_lst, bool new_playlist);
	virtual void psl_prepare_tracks_for_playlist(const QStringList& file_paths, bool new_playlist);


	/* append tracks after current played track in playlist */
	virtual void psl_play_next_all_tracks();
	virtual void psl_play_next_tracks(const IdxList& idx_lst);


	/* append tracks after last track in playlist */
	virtual void psl_append_all_tracks();
	virtual void psl_append_tracks(const IdxList& idx_lst);


	/* triggered from outside, when something important has been changed*/
	virtual void psl_metadata_changed(const MetaData&);

	/* triggered from outside, when playing time*/
	virtual void psl_dur_changed(const MetaData&);


	/* triggered by tagedit */
	virtual void psl_metadata_id3_changed(const MetaDataList&, const MetaDataList&);

	// calls fetch_by_filter and emits
	virtual void psl_filter_changed(const Filter&, bool force=false);

	/* write new rating to database */
	virtual void psl_track_rating_changed(int idx, int rating);
	virtual void psl_album_rating_changed(int idx, int rating);

	/* a searchfilter has been entered, nothing is emitted */
	virtual void fetch_by_filter(const Filter& filter, bool force);


	virtual void delete_tracks(const MetaDataList& v_md, TrackDeletionMode mode)=0;

	virtual void delete_tracks_by_idx(const IdxList& idxs, TrackDeletionMode mode);
	virtual void delete_all_tracks();
	virtual void delete_current_tracks(TrackDeletionMode mode);

	virtual void insert_tracks(const MetaDataList& v_md);
	virtual void import_files(const QStringList& files);


	/* Check for current selected artist if out of date and
	 * fetch new data */
	virtual void refresh_artist()=0;
	virtual void refresh_albums()=0;
	virtual void refresh_tracks()=0;


protected slots:
	virtual void _sl_sortorder_changed();


protected:

	EngineHandler*		_engine=nullptr;
	PlaylistHandler*	_playlist=nullptr;

	MetaDataList        _vec_md;
	AlbumList			_vec_albums;
	ArtistList			_vec_artists;

	// contains ID for artists, albums, tracks
	IDList				_selected_artists;
	IDList				_selected_albums;
	IDList				_selected_tracks;

	Filter				_filter;
	LibSortOrder		_sortorder;


	/* Emit 3 signals with shown artists, shown album, shown tracks */
	virtual void 		emit_stuff();


	virtual void		get_all_artists(ArtistList& artists, LibSortOrder so)=0;
	virtual void		get_all_artists_by_searchstring(Filter filter, ArtistList& artists, LibSortOrder so)=0;

	virtual void		get_all_albums(AlbumList& albums, LibSortOrder so)=0;
	virtual void		get_all_albums_by_artist(IDList artist_ids, AlbumList& albums, Filter filter, LibSortOrder so)=0;
	virtual void		get_all_albums_by_searchstring(Filter filter, AlbumList& albums, LibSortOrder so)=0;


	virtual void		get_all_tracks(MetaDataList& v_md, LibSortOrder so)=0;
	virtual void		get_all_tracks(const QStringList& paths, MetaDataList& v_md)=0;
	virtual void		get_all_tracks_by_artist(IDList artist_ids, MetaDataList& v_md, Filter filter, LibSortOrder so)=0;
	virtual	void		get_all_tracks_by_album(IDList album_ids, MetaDataList& v_md, Filter filter, LibSortOrder so)=0;
	virtual void		get_all_tracks_by_searchstring(Filter filter, MetaDataList& v_md, LibSortOrder so)=0;

	virtual void		get_album_by_id(int album_id, Album& album)=0;
	virtual void		get_artist_by_id(int artist_id, Artist& artist)=0;

	virtual void		update_track(const MetaData& md)=0;
	virtual void		update_album(const Album& album)=0;


private:

	void set_playlist_action_after_double_click();

	virtual void restore_artist_selection();
	virtual void restore_track_selection();
	virtual void restore_album_selection();

	virtual MetaDataList change_track_selection(const IdxList& idx_list);
	virtual void change_artist_selection(const IdxList& idx_list);
	virtual void change_album_selection(const IdxList& idx_list);

};

#endif // ABSTRACTLIBRARY_H
