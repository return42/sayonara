/* AbstractLibrary.cpp */

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

#include "AbstractLibrary.h"

#include "Components/Playlist/PlaylistHandler.h"
#include "Components/PlayManager/PlayManager.h"
#include "Components/Tagging/ChangeNotifier.h"
#include "Components/Tagging/Editor.h"

#include "Utils/typedefs.h"
#include "Utils/MetaData/Genre.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Language.h"
#include "Utils/Library/SearchMode.h"

#include <QHash>

struct AbstractLibrary::Private
{
	SP::Set<ArtistID> selected_artists;
	SP::Set<AlbumID> selected_albums;
	SP::Set<TrackID> selected_tracks;

	MetaDataList current_tracks;

	Tagging::Editor* tag_edit=nullptr;
	Playlist::Handler* playlist=nullptr;

	Library::Sortings sortorder;
	Library::Filter filter;
};

AbstractLibrary::AbstractLibrary(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	m = Pimpl::make<Private>();

	m->playlist = Playlist::Handler::instance();
	m->sortorder = _settings->get(Set::Lib_Sorting);

	m->filter.set_mode(Library::Filter::Fulltext);
	m->filter.set_filtertext("");

	Tagging::ChangeNotifier* mdcn = Tagging::ChangeNotifier::instance();
	connect(mdcn, &Tagging::ChangeNotifier::sig_metadata_changed,
			this, &AbstractLibrary::metadata_id3_changed);
}

AbstractLibrary::~AbstractLibrary() {}

void AbstractLibrary::load ()
{
	Set::listen(Set::Lib_Sorting, this, &AbstractLibrary::_sl_sortorder_changed);

	m->filter.clear();

	get_all_artists(_artists, m->sortorder);
	get_all_albums(_albums, m->sortorder);
	get_all_tracks(_tracks, m->sortorder);

	emit_stuff();
}


void AbstractLibrary::emit_stuff()
{
	emit sig_all_albums_loaded();
	emit sig_all_artists_loaded();
	emit sig_all_tracks_loaded();
}

void AbstractLibrary::refetch()
{
	m->selected_albums.clear();
	m->selected_artists.clear();
	m->selected_tracks.clear();
	m->filter.clear();

	_albums.clear();
	_artists.clear();
	_tracks.clear();

	get_all_tracks(_tracks, m->sortorder);
	get_all_albums(_albums, m->sortorder);
	get_all_artists(_artists, m->sortorder);

	emit_stuff();
}


void AbstractLibrary::refresh()
{
	IndexSet sel_artists_idx, sel_albums_idx, sel_tracks_idx;

	IDSet sel_artists = m->selected_artists;
	IDSet sel_albums = m->selected_albums;
	IDSet sel_tracks = m->selected_tracks;

	fetch_by_filter(m->filter, true);

	for(int i=0; i<_artists.count(); i++){
		if(sel_artists.contains(_artists[i].id)) {
			sel_artists_idx.insert(i);
		}
	}

	change_artist_selection(sel_artists_idx);

	for(int i=0; i<_albums.count(); i++){
		if(sel_albums.contains(_albums[i].id)) {
			sel_albums_idx.insert(i);
		}
	}

	change_album_selection(sel_albums_idx);

	for(int i=0; i<_tracks.count(); i++)
	{
		if(sel_tracks.contains(_tracks[i].id)) {
			sel_tracks_idx.insert(i);
		}
	}

	emit_stuff();

	if(sel_tracks_idx.size() > 0)
	{
		change_track_selection(sel_tracks_idx);
	}
}


void AbstractLibrary::prepare_tracks_for_playlist(bool new_playlist)
{
	if(!new_playlist) {
		m->playlist->create_playlist( current_tracks() );
	}

	else {
		m->playlist->create_playlist( current_tracks(),
									  m->playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}

void AbstractLibrary::prepare_tracks_for_playlist(const QStringList& paths, bool new_playlist)
{
	if(!new_playlist) {
		m->playlist->create_playlist(paths);
	}

	else {
		m->playlist->create_playlist(paths, m->playlist->request_new_playlist_name());
	}

	set_playlist_action_after_double_click();
}


void AbstractLibrary::set_playlist_action_after_double_click()
{
	PlayManagerPtr play_manager = PlayManager::instance();

	if(_settings->get(Set::Lib_DC_DoNothing)){
		return;
	}

	else if(_settings->get(Set::Lib_DC_PlayIfStopped))
	{
		if(play_manager->playstate() != PlayState::Playing){
			m->playlist->change_track(0, m->playlist->current_index());
		}
	}

	else {
		m->playlist->change_track(0, m->playlist->current_index());
	}
}


void AbstractLibrary::play_next_all_tracks()
{
	m->playlist->play_next(_tracks);
}

void AbstractLibrary::play_next_current_tracks()
{
	m->playlist->play_next( current_tracks() );
}


void AbstractLibrary::append_all_tracks()
{
	m->playlist->append_tracks(_tracks, m->playlist->current_index());
}

void AbstractLibrary::append_current_tracks()
{
	m->playlist->append_tracks(current_tracks(), m->playlist->current_index());
}

void AbstractLibrary::change_artist_selection(const IndexSet& indexes)
{
	SP::Set<ArtistID> selected_artists;
	for(auto it=indexes.begin(); it!=indexes.end(); it++){
		int idx = *it;
		const Artist& artist = _artists[idx];
		selected_artists.insert(artist.id);
	}

	if(selected_artists == m->selected_artists)
	{
		return;
	}

	_albums.clear();
	_tracks.clear();

	m->selected_artists = selected_artists;

	if(m->selected_artists.size() > 0) {
		get_all_tracks_by_artist(m->selected_artists.toList(), _tracks, m->filter, m->sortorder);
		get_all_albums_by_artist(m->selected_artists.toList(), _albums, m->filter, m->sortorder);
	}

	else if(!m->filter.cleared()) {
		get_all_tracks_by_searchstring(m->filter, _tracks, m->sortorder);
		get_all_albums_by_searchstring(m->filter, _albums, m->sortorder);
		get_all_artists_by_searchstring(m->filter, _artists, m->sortorder);
	}

	else{
		get_all_tracks(_tracks, m->sortorder);
		get_all_albums(_albums, m->sortorder);
	}
}



const MetaDataList& AbstractLibrary::tracks() const
{
	return _tracks;
}

const AlbumList& AbstractLibrary::albums() const
{
	return _albums;
}

const ArtistList& AbstractLibrary::artists() const
{
	return _artists;
}

const MetaDataList& AbstractLibrary::current_tracks() const
{
	if(m->selected_tracks.isEmpty()){
		return _tracks;
	}

	return m->current_tracks;
}

const IDSet& AbstractLibrary::selected_tracks() const
{
	return m->selected_tracks;
}

const IDSet& AbstractLibrary::selected_albums() const
{
	return m->selected_albums;
}

const IDSet& AbstractLibrary::selected_artists() const
{
	return m->selected_artists;
}


Library::Filter AbstractLibrary::filter() const
{
	return m->filter;
}

void AbstractLibrary::change_filter(Library::Filter filter, bool force)
{
	QString filtertext = filter.filtertext();

	if(filtertext.size() < 3){
		filter.clear();
	}

	else
	{
		if(filter.mode() == Library::Filter::Fulltext)
		{
			Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);
			filtertext = Library::Util::convert_search_string(filtertext, mask);
		}

		filtertext.prepend("%");
		filtertext.append("%");

		filter.set_filtertext(filtertext);
	}

	fetch_by_filter(filter, force);
	emit_stuff();
}

void AbstractLibrary::selected_artists_changed(const IndexSet& indexes)
{
	change_artist_selection(indexes);

	emit sig_all_albums_loaded();
	emit sig_all_tracks_loaded();
}


void AbstractLibrary::change_album_selection(const IndexSet& indexes)
{
	SP::Set<AlbumID> selected_albums;
	bool show_album_artists = _settings->get(Set::Lib_ShowAlbumArtists);

	for(auto it=indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		if(idx >= _albums.count()){
			break;
		}

		const Album& album = _albums[idx];
		selected_albums.insert(album.id);
	}

	if(selected_albums == m->selected_albums) {
		return;
	}

	_tracks.clear();
	m->selected_albums = selected_albums;

	// only show tracks of selected album / artist
	if(m->selected_artists.size() > 0)
	{
		if(m->selected_albums.size() > 0)
		{
			MetaDataList v_md;

			get_all_tracks_by_album(m->selected_albums.toList(), v_md, m->filter, m->sortorder);

			// filter by artist

			for(const MetaData& md : v_md) {
				ArtistID artist_id;
				if(show_album_artists){
					artist_id = md.album_artist_id();
				}

				else{
					artist_id = md.artist_id;
				}

				if(m->selected_artists.contains(artist_id)){
					_tracks << std::move(md);
				}
			}
		}

		else{
			get_all_tracks_by_artist(m->selected_artists.toList(), _tracks, m->filter, m->sortorder);
		}
	}

	// only album is selected
	else if(m->selected_albums.size() > 0) {
		get_all_tracks_by_album(m->selected_albums.toList(), _tracks, m->filter, m->sortorder);
	}

	// neither album nor artist, but searchstring
	else if(!m->filter.cleared()) {
		get_all_tracks_by_searchstring(m->filter, _tracks, m->sortorder);
	}

	// no album, no artist, no searchstring
	else{
		get_all_tracks(_tracks, m->sortorder);
	}
}


void AbstractLibrary::selected_albums_changed(const IndexSet& indexes)
{
	change_album_selection(indexes);
	emit sig_all_tracks_loaded();
}


void AbstractLibrary::change_track_selection(const IndexSet& indexes)
{
	m->selected_tracks.clear();
	m->current_tracks.clear();

	for(int idx : indexes)
	{
		if(idx < 0 || idx >= _tracks.count()){
			continue;
		}

		const MetaData& md = _tracks[idx];

		m->current_tracks << md;
		m->selected_tracks.insert(md.id);
	}
}


void AbstractLibrary::selected_tracks_changed(const IndexSet& indexes)
{
	change_track_selection(indexes);
}


void AbstractLibrary::fetch_by_filter(Library::Filter filter, bool force)
{
	if( (m->filter == filter) &&
		(m->selected_artists.empty()) &&
		(m->selected_albums.empty()) &&
		!force)
	{
		return;
	}

	m->filter = filter;

	_albums.clear();
	_artists.clear();
	_tracks.clear();

	m->selected_albums.clear();
	m->selected_artists.clear();

	if(m->filter.cleared()) {
		get_all_artists(_artists, m->sortorder);
		get_all_albums(_albums, m->sortorder);
		get_all_tracks(_tracks, m->sortorder);
	}

	else {
		get_all_artists_by_searchstring(m->filter, _artists, m->sortorder);
		get_all_albums_by_searchstring(m->filter, _albums, m->sortorder);
		get_all_tracks_by_searchstring(m->filter, _tracks, m->sortorder);
	}
}


void AbstractLibrary::_sl_sortorder_changed()
{
	Library::Sortings so = _settings->get(Set::Lib_Sorting);

	// artist sort order has changed
	if(so.so_artists != m->sortorder.so_artists)
	{
		m->sortorder = so;
		_artists.clear();

		if(!m->filter.cleared()) {
			get_all_artists_by_searchstring(m->filter, _artists, m->sortorder);
		}

		else{
			get_all_artists(_artists, m->sortorder);
		}

		emit sig_all_artists_loaded();
	}


	// album sort order has changed
	if(so.so_albums != m->sortorder.so_albums)
	{
		m->sortorder = so;
		_albums.clear();

		// selected artists and maybe filter
		if (m->selected_artists.size() > 0) {
			get_all_albums_by_artist(m->selected_artists.toList(), _albums, m->filter, m->sortorder);
		}

		// only filter
		else if( !m->filter.cleared() ) {
			get_all_albums_by_searchstring(m->filter, _albums, m->sortorder);
		}

		// all albums
		else{
			get_all_albums(_albums, m->sortorder);
		}

		emit sig_all_albums_loaded();
	}


	// track sort order has changed
	if(so.so_tracks != m->sortorder.so_tracks) {
		m->sortorder = so;
		_tracks.clear();

		if(m->selected_albums.size() > 0) {
			get_all_tracks_by_album(m->selected_albums.toList(), _tracks, m->filter, m->sortorder);
		}
		else if(m->selected_artists.size() > 0) {
			get_all_tracks_by_artist(m->selected_artists.toList(), _tracks, m->filter, m->sortorder);
		}

		else if(!m->filter.cleared()) {
			get_all_tracks_by_searchstring(m->filter, _tracks, m->sortorder);
		}

		else {
			get_all_tracks(_tracks, m->sortorder);
		}

		emit sig_all_tracks_loaded();
	}
}


void AbstractLibrary::change_track_rating(int idx, int rating)
{
	_tracks[idx].rating = rating;
	update_track(_tracks[idx]);
}


void AbstractLibrary::change_album_rating(int idx, int rating)
{
	_albums[idx].rating = rating;
	update_album(_albums[idx]);
}


void AbstractLibrary::metadata_id3_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	// id -> idx
	QHash<TrackID, int> md_map;

	for(int i=0; i<_tracks.count(); i++)
	{
		TrackID id = _tracks[i].id;
		md_map[id] = i;
	}

	// check for new artists and albums
	for(int i=0; i<v_md_old.count(); i++)
	{
		TrackID id = v_md_old[i].id;
		ArtistID new_artist_id = v_md_new[i].artist_id;
		AlbumID new_album_id = v_md_new[i].album_id;

		if( v_md_old[i].artist_id != new_artist_id )
		{
			m->selected_artists.insert(new_artist_id);
		}

		if( v_md_old[i].album_id != new_album_id)
		{
			m->selected_albums.insert(new_album_id);
		}

		if(md_map.contains(id))
		{
			int val = md_map[id];
			_tracks[val] = v_md_new[i];
		}
	}

	refresh();
	emit_stuff();
}

void AbstractLibrary::update_tracks(const MetaDataList& v_md)
{
	for(const MetaData& md : v_md){
		update_track(md);
	}

	refresh();
}

Library::Sortings AbstractLibrary::sortorder() const
{
	return m->sortorder;
}

Tagging::Editor* AbstractLibrary::tag_edit()
{
	if(!m->tag_edit){
		m->tag_edit = new Tagging::Editor(this);
		connect(m->tag_edit, &Tagging::Editor::finished, this, &AbstractLibrary::refresh);
		connect(m->tag_edit, &Tagging::Editor::sig_progress, [=](int progress){
			emit sig_reloading_library(Lang::get(Lang::ReloadLibrary), progress);
		});
	}

	return m->tag_edit;
}

void AbstractLibrary::insert_tracks(const MetaDataList &v_md)
{
	Q_UNUSED(v_md)
	refresh();
}

void AbstractLibrary::import_files(const QStringList &files)
{
	Q_UNUSED(files)
}


void AbstractLibrary::delete_current_tracks(Library::TrackDeletionMode mode)
{
	if(mode == Library::TrackDeletionMode::None) return;
	delete_tracks( current_tracks(), mode);
}


void AbstractLibrary::delete_all_tracks()
{
	MetaDataList v_md;
	get_all_tracks(v_md, m->sortorder);
	delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);
}


void AbstractLibrary::delete_tracks(const MetaDataList& v_md, Library::TrackDeletionMode mode)
{
	if(mode == Library::TrackDeletionMode::None) return;

	QString file_entry = Lang::get(Lang::Entries);
	QString answer_str;

	int n_fails = 0;
	if(mode == Library::TrackDeletionMode::AlsoFiles){
		file_entry = Lang::get(Lang::Files);
		for( const MetaData& md : v_md ){
			QFile f(md.filepath());
			if(!f.remove()){
				n_fails++;
			}
		}
	}

	if(n_fails == 0) {
		answer_str = tr("All %1 could be removed").arg(file_entry);
	}

	else {
		answer_str = tr("%1 of %2 %3 could not be removed").arg(n_fails).arg(v_md.size()).arg(file_entry);
	}

	emit sig_delete_answer(answer_str);
	Tagging::ChangeNotifier::instance()->delete_metadata(v_md);

	refresh();
}


void AbstractLibrary::delete_tracks_by_idx(const IndexSet& indexes, Library::TrackDeletionMode mode)
{
	if(mode == Library::TrackDeletionMode::None) return;

	MetaDataList v_md, v_md_old, v_md_changed;
	for(auto it = indexes.begin(); it != indexes.end(); it++){
		int idx = *it;
		v_md.push_back(_tracks[idx]);
	}

	delete_tracks(v_md, mode);
}


void AbstractLibrary::add_genre(SP::Set<ID> ids, const QString& genre)
{
	Genre g(genre);
	MetaDataList v_md;
	get_all_tracks(v_md, Library::Sortings());

	tag_edit()->set_metadata(v_md);

	for(int i=0; i<v_md.count(); i++)
	{
		if( ids.contains(v_md[i].id) ){
			tag_edit()->add_genre(i, genre);
		}
	}

	tag_edit()->commit();
}


void AbstractLibrary::delete_genre(const QString& genre)
{
	MetaDataList v_md;

	sp_log(Log::Debug, this) << "Delete genre: Fetch all tracks";
	get_all_tracks(v_md, Library::Sortings());
	sp_log(Log::Debug, this) << "Delete genre: Set Metadata";
	tag_edit()->set_metadata(v_md);

	for(int i=0; i<v_md.count(); i++)
	{
		tag_edit()->delete_genre(i, genre);
	}

	tag_edit()->commit();
}

void AbstractLibrary::rename_genre(const QString& genre, const QString& new_genre)
{
	MetaDataList v_md;
	Genre g(genre);

	sp_log(Log::Debug, this) << "Rename genre: Fetch all tracks";
	get_all_tracks(v_md, Library::Sortings());
	tag_edit()->set_metadata(v_md);

	for(int i=0; i<v_md.count(); i++)
	{
		if(v_md[i].has_genre(g)){
			tag_edit()->delete_genre(i, genre);
			tag_edit()->add_genre(i, new_genre);
		}
	}

	tag_edit()->commit();
}
