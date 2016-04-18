/* GUI_AbstractLibrary.cpp */

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

#include "GUI_AbstractLibrary.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "GUI/Helper/GlobalMessage/Message.h"

#include <QKeySequence>

GUI_AbstractLibrary::GUI_AbstractLibrary(AbstractLibrary* library, QWidget *parent) :
	SayonaraWidget(parent)
{
	_library = library;
	_info_dialog = new GUI_InfoDialog(this);

	_shown_cols_albums = _settings->get(Set::Lib_ColsAlbum);
	_shown_cols_artist = _settings->get(Set::Lib_ColsArtist);
	_shown_cols_tracks = _settings->get(Set::Lib_ColsTitle);
}

GUI_AbstractLibrary::~GUI_AbstractLibrary(){
	delete _album_model;
	delete _album_delegate;
	delete _artist_model;
	delete _artist_delegate;
	delete _track_model;
	delete _track_delegate;
}

void GUI_AbstractLibrary::init_finished(){

	init_headers();

	connect(_library, &AbstractLibrary::sig_all_artists_loaded, this, &GUI_AbstractLibrary::lib_fill_artists);
	connect(_library, &AbstractLibrary::sig_all_albums_loaded, this, &GUI_AbstractLibrary::lib_fill_albums);
	connect(_library, &AbstractLibrary::sig_all_tracks_loaded, this,	&GUI_AbstractLibrary::lib_fill_tracks);
	connect(_library, &AbstractLibrary::sig_track_mime_data_available, this, &GUI_AbstractLibrary::track_info_available);
	connect(_library, &AbstractLibrary::sig_delete_answer, this, &GUI_AbstractLibrary::show_delete_answer);

	connect(_lv_album, &LibraryViewAlbum::doubleClicked, this, &GUI_AbstractLibrary::album_dbl_clicked);
	connect(_lv_album, &LibraryViewAlbum::sig_sel_changed, this, &GUI_AbstractLibrary::album_sel_changed);
	connect(_lv_album, &LibraryViewAlbum::sig_middle_button_clicked, this, &GUI_AbstractLibrary::album_middle_clicked);
	connect(_lv_album, &LibraryViewAlbum::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_album_changed);
	connect(_lv_album, &LibraryViewAlbum::sig_columns_changed, this, &GUI_AbstractLibrary::columns_album_changed);
	connect(_lv_album, &LibraryViewAlbum::sig_edit_clicked, this, &GUI_AbstractLibrary::edit_album);
	connect(_lv_album, &LibraryViewAlbum::sig_info_clicked, this, &GUI_AbstractLibrary::info_album);
	connect(_lv_album, &LibraryViewAlbum::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_album);
	connect(_lv_album, &LibraryViewAlbum::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
	connect(_lv_album, &LibraryViewAlbum::sig_append_clicked, this, &GUI_AbstractLibrary::append);
	connect(_lv_album, &LibraryViewAlbum::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_album);

	connect(_lv_artist, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::artist_dbl_clicked);
	connect(_lv_artist, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::artist_sel_changed);
	connect(_lv_artist, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::artist_middle_clicked);
	connect(_lv_artist, &LibraryView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_artist_changed);
	connect(_lv_artist, &LibraryView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_artist_changed);
	connect(_lv_artist, &LibraryView::sig_edit_clicked, this, &GUI_AbstractLibrary::edit_artist);
	connect(_lv_artist, &LibraryView::sig_info_clicked, this, &GUI_AbstractLibrary::info_artist);
	connect(_lv_artist, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_artist);
	connect(_lv_artist, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
	connect(_lv_artist, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append);
	connect(_lv_artist, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_artist);

	connect(_lv_tracks, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::track_dbl_clicked);
	connect(_lv_tracks, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::track_sel_changed);
	connect(_lv_tracks, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::tracks_middle_clicked);
	connect(_lv_tracks, &LibraryView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_title_changed);
	connect(_lv_tracks, &LibraryView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_title_changed);
	connect(_lv_tracks, &LibraryView::sig_edit_clicked, this, &GUI_AbstractLibrary::edit_tracks);
	connect(_lv_tracks, &LibraryView::sig_info_clicked, this, &GUI_AbstractLibrary::info_tracks);
	connect(_lv_tracks, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_tracks);
	connect(_lv_tracks, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next_tracks);
	connect(_lv_tracks, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append_tracks);
	connect(_lv_tracks, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_tracks);

	connect(_btn_clear, &QPushButton::clicked, this, &GUI_AbstractLibrary::clear_button_pressed);

	// overloaded
	connect(_combo_search, combo_current_index_changed_int, this, &GUI_AbstractLibrary::combo_search_changed);

	REGISTER_LISTENER(Set::Lib_LiveSearch, _sl_live_search_changed);
}

void GUI_AbstractLibrary::set_lib_chooser(const QMap<QString, QIcon>& items){
	if(!_combo_libchooser) return;

	for(const QString& key : items.keys()){
		_combo_libchooser->addItem(items[key], key);
	}

	_combo_libchooser->setIconSize(QSize(16, 16));
	_combo_libchooser->view()->setIconSize(QSize(16, 16));
}

void GUI_AbstractLibrary::language_changed(){

	_header_names_tracks.clear();
	_header_names_albums.clear();
	_header_names_artists.clear();

	_header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize");
	_header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year");
	_header_names_artists << "#" << tr("Artist") << tr("#Tracks");

	_album_model->set_new_header_names(_header_names_albums);
	_artist_model->set_new_header_names(_header_names_artists);
	_track_model->set_new_header_names(_header_names_tracks);

	_lv_album->rc_header_menu_retranslate();
	_lv_artist->rc_header_menu_retranslate();
	_lv_tracks->rc_header_menu_retranslate();
}

void GUI_AbstractLibrary::init_headers(){

	LibSortOrder so = _settings->get(Set::Lib_Sorting);

	_header_names_tracks << "#" << tr("Title") << tr("Artist") << tr("Album") << tr("Year") << tr("Dur.") << tr("Bitrate") << tr("Filesize") << tr("Rating");
	_header_names_albums << "#" << tr("Album") << tr("Duration") << tr("#Tracks") << tr("Year") << tr("Rating");
	_header_names_artists << "#" << tr("Artist") << tr("#Tracks");

	ColumnHeader t_h0(_header_names_tracks[0], true, SortOrder::TrackNumAsc, SortOrder::TrackNumDesc, 25);
	ColumnHeader t_h1(_header_names_tracks[1], false, SortOrder::TrackTitleAsc, SortOrder::TrackTitleDesc, 0.4, 200);
	ColumnHeader t_h2(_header_names_tracks[2], true, SortOrder::TrackArtistAsc, SortOrder::TrackArtistDesc, 0.3, 160);
	ColumnHeader t_h3(_header_names_tracks[3], true, SortOrder::TrackAlbumAsc, SortOrder::TrackAlbumDesc, 0.3, 160);
	ColumnHeader t_h4(_header_names_tracks[4], true, SortOrder::TrackYearAsc, SortOrder::TrackYearDesc, 50);
	ColumnHeader t_h5(_header_names_tracks[5], true, SortOrder::TrackLenghtAsc, SortOrder::TrackLengthDesc, 50);
	ColumnHeader t_h6(_header_names_tracks[6], true, SortOrder::TrackBitrateAsc, SortOrder::TrackBitrateDesc, 75);
	ColumnHeader t_h7(_header_names_tracks[7], true, SortOrder::TrackSizeAsc, SortOrder::TrackSizeDesc, 75);
	ColumnHeader t_h8(_header_names_tracks[8], true, SortOrder::TrackRatingAsc, SortOrder::TrackRatingDesc, 80);

	ColumnHeader al_h0(_header_names_albums[0], true, SortOrder::NoSorting, SortOrder::NoSorting, 20);
	ColumnHeader al_h1(_header_names_albums[1], false, SortOrder::AlbumNameAsc, SortOrder::AlbumNameDesc, 1.0, 160);
	ColumnHeader al_h2(_header_names_albums[2], true, SortOrder::AlbumDurationAsc, SortOrder::AlbumDurationDesc, 90);
	ColumnHeader al_h3(_header_names_albums[3], true, SortOrder::AlbumTracksAsc, SortOrder::AlbumTracksDesc, 80);
	ColumnHeader al_h4(_header_names_albums[4], true, SortOrder::AlbumYearAsc, SortOrder::AlbumYearDesc, 50);
	ColumnHeader al_h5(_header_names_albums[5], true, SortOrder::AlbumRatingAsc, SortOrder::AlbumRatingDesc, 80);

	ColumnHeader ar_h0(_header_names_artists[0], true, SortOrder::NoSorting, SortOrder::NoSorting, 20);
	ColumnHeader ar_h1(_header_names_artists[1], false, SortOrder::ArtistNameAsc, SortOrder::ArtistNameDesc, 1.0, 160 );
	ColumnHeader ar_h2(_header_names_artists[2], true, SortOrder::ArtistTrackcountAsc, SortOrder::ArtistTrackcountDesc, 80);

	_track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;
	_album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4 << al_h5;
	_artist_columns << ar_h0 << ar_h1 << ar_h2;

	_album_model = new LibraryItemModelAlbums(_album_columns);
	_artist_model = new LibraryItemModelArtists(_artist_columns);
	_track_model = new LibraryItemModelTracks(_track_columns);

	_album_delegate = new LibraryItemDelegateAlbums(_lv_album, true);
	_artist_delegate = new LibraryItemDelegateArtists(_lv_artist);
	_track_delegate = new LibraryItemDelegateTracks(_lv_tracks, true);

	connect(_album_delegate, &LibraryRatingDelegate::sig_rating_changed, this, &GUI_AbstractLibrary::album_rating_changed);
	connect(_track_delegate, &LibraryRatingDelegate::sig_rating_changed, this, &GUI_AbstractLibrary::title_rating_changed);


	_lv_tracks->setModel(_track_model);
	_lv_tracks->setAbstractModel(_track_model);
	_lv_tracks->setItemDelegate(_track_delegate);
	_lv_tracks->setAlternatingRowColors(true);
	_lv_tracks->setDragEnabled(true);
	_lv_tracks->set_table_headers(_track_columns, so.so_tracks);
	_lv_tracks->rc_header_menu_init(_shown_cols_tracks);

	_lv_artist->setModel(_artist_model);
	_lv_artist->setAbstractModel(_artist_model);
	_lv_artist->setItemDelegate(_artist_delegate);
	_lv_artist->setAlternatingRowColors(true);
	_lv_artist->setDragEnabled(true);
	_lv_artist->set_table_headers(_artist_columns, so.so_artists);
	_lv_artist->rc_header_menu_init(_shown_cols_artist);

	_lv_album->setModel(_album_model);
	_lv_album->setAbstractModel(_album_model);
	_lv_album->setItemDelegate(_album_delegate);
	_lv_album->setAlternatingRowColors(true);
	_lv_album->setDragEnabled(true);
	_lv_album->set_table_headers(_album_columns, so.so_albums);
	_lv_album->rc_header_menu_init(_shown_cols_albums);

}

void GUI_AbstractLibrary::init_shortcuts()
{

}


void GUI_AbstractLibrary::text_line_edited(const QString &search){

	if(search.startsWith("f:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(0);
		_le_search->setText("");
	}

	else if(search.startsWith("g:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(1);
		_le_search->setText("");
	}

	else if(search.startsWith("p:", Qt::CaseInsensitive)) {
		_combo_search->setCurrentIndex(2);
		_le_search->setText("");
	}

	Filter filter;
	switch( _combo_search->currentIndex() ) {

		case 1:
			filter.mode = Filter::Mode::Genre;
			break;

		case 2:
			filter.mode = Filter::Mode::Filename;
			break;

		case 0:
		default:
			filter.mode = Filter::Mode::Fulltext;
			break;
	}

	if(search.size() < 3){
		filter.cleared = true;
	}

	else{
		filter.filtertext = QString("%") + search + QString("%");
		filter.cleared = false;
	}

	_cur_searchfilter = filter;

	_library->psl_filter_changed(filter);
}


void GUI_AbstractLibrary::clear_button_pressed() {

	disconnect(_le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);

	_combo_search->setCurrentIndex(0);
	_le_search->setText("");
	_library->refetch();

	connect(_le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
}


void GUI_AbstractLibrary::combo_search_changed(int idx) {

	switch(idx) {

		case 1:
			_cur_searchfilter.mode = Filter::Mode::Genre;
			break;

		case 2:
			_cur_searchfilter.mode = Filter::Mode::Filename;
			break;

		case 0:
		default:
			_cur_searchfilter.mode = Filter::Mode::Fulltext; break;
	}

	_library->psl_filter_changed(_cur_searchfilter);
}

void GUI_AbstractLibrary::return_pressed() {
	text_line_edited(_le_search->text());
}


void GUI_AbstractLibrary::refresh(){
	_library->refresh();
}


void GUI_AbstractLibrary::lib_fill_tracks(const MetaDataList& v_md) {

	_lv_tracks->fill<MetaDataList, LibraryItemModelTracks>(v_md);
	_artist_model->set_mimedata(v_md);
	_album_model->set_mimedata(v_md);
}


void GUI_AbstractLibrary::lib_fill_albums(const AlbumList& albums) {

	_lv_album->fill<AlbumList, LibraryItemModelAlbums>(albums);
}


void GUI_AbstractLibrary::lib_fill_artists(const ArtistList& artists) {

	_lv_artist->fill<ArtistList, LibraryItemModelArtists>(artists);
}


void GUI_AbstractLibrary::track_info_available(const MetaDataList& v_md) {
	_track_model->set_mimedata(v_md);
}


void GUI_AbstractLibrary::artist_sel_changed(const SP::Set<int>& lst) {
	_library->psl_selected_artists_changed(lst);
}


void GUI_AbstractLibrary::album_sel_changed(const SP::Set<int>& lst) {
	_library->psl_selected_albums_changed(lst);
}

void GUI_AbstractLibrary::track_sel_changed(const SP::Set<int>& lst) {
	_library->psl_selected_tracks_changed(lst);
}


void GUI_AbstractLibrary::artist_middle_clicked(const QPoint& pt) {
	Q_UNUSED(pt)
	_library->psl_prepare_tracks_for_playlist(true);
}


void GUI_AbstractLibrary::album_middle_clicked(const QPoint& pt) {
	Q_UNUSED(pt)
	_library->psl_prepare_tracks_for_playlist(true);
}

void GUI_AbstractLibrary::tracks_middle_clicked(const QPoint& pt) {
	Q_UNUSED(pt)
	_library->psl_prepare_tracks_for_playlist(_lv_tracks->get_selections(), true);
}


void GUI_AbstractLibrary::album_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_album_for_playlist(idx.row(), false);
}

void GUI_AbstractLibrary::artist_dbl_clicked(const QModelIndex & idx) {
	_library->psl_prepare_artist_for_playlist(idx.row(), false);
}

void GUI_AbstractLibrary::track_dbl_clicked(const QModelIndex& idx) {

	SP::Set<int> indexes = _lv_tracks->get_selections();

	if( indexes.isEmpty() ) {
		indexes.insert(idx.row());
	}

	_library->psl_prepare_tracks_for_playlist(indexes, false);
}


void  GUI_AbstractLibrary::columns_album_changed(const BoolList& list) {
	_shown_cols_albums = list;
	_settings->set(Set::Lib_ColsAlbum, list);
}


void  GUI_AbstractLibrary::columns_artist_changed(const BoolList& list) {
	_shown_cols_artist = list;
	_settings->set(Set::Lib_ColsArtist, list);
}


void  GUI_AbstractLibrary::columns_title_changed(const BoolList& list) {
	_shown_cols_tracks = list;
	_settings->set(Set::Lib_ColsTitle, list);
}



void GUI_AbstractLibrary::sortorder_artist_changed(SortOrder s) {

	_lv_artist->save_selections();

	LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_artists = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_album_changed(SortOrder s) {

	_lv_album->save_selections();

   LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_albums = s;

	_settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_title_changed(SortOrder s) {

	_lv_tracks->save_selections();

	LibSortOrder so = _settings->get(Set::Lib_Sorting);
	so.so_tracks = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::edit_album() {
	_info_dialog->set_metadata(_lv_album->get_selected_metadata(), GUI_InfoDialog::Mode::Albums);
	_info_dialog->show(GUI_InfoDialog::TabEdit);
}

void GUI_AbstractLibrary::edit_artist() {
	_info_dialog->set_metadata(_lv_artist->get_selected_metadata(), GUI_InfoDialog::Mode::Artists);
	_info_dialog->show(GUI_InfoDialog::TabEdit);
}

void GUI_AbstractLibrary::edit_tracks() {
	_info_dialog->set_metadata(_lv_tracks->get_selected_metadata(), GUI_InfoDialog::Mode::Tracks);
	_info_dialog->show(GUI_InfoDialog::TabEdit);
}


void GUI_AbstractLibrary::info_album() {
	_info_dialog->set_metadata(_lv_album->get_selected_metadata(), GUI_InfoDialog::Mode::Albums);
	_info_dialog->show(GUI_InfoDialog::TabInfo);
}

void GUI_AbstractLibrary::info_artist() {

	if(!_info_dialog){
		_info_dialog = new GUI_InfoDialog(this);
	}
	_info_dialog->set_metadata(_lv_artist->get_selected_metadata(), GUI_InfoDialog::Mode::Artists);
	_info_dialog->show(GUI_InfoDialog::TabInfo);
}

void GUI_AbstractLibrary::info_tracks() {
	if(!_info_dialog){

	}
	_info_dialog->set_metadata(_lv_tracks->get_selected_metadata(), GUI_InfoDialog::Mode::Tracks);
	_info_dialog->show(GUI_InfoDialog::TabInfo);
}


void GUI_AbstractLibrary::delete_album() {

	int n_tracks = _track_model->rowCount();
	Library::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_library->delete_current_tracks(answer);

}

void GUI_AbstractLibrary::delete_artist() {

	int n_tracks = _track_model->rowCount();
	Library::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_library->delete_current_tracks(answer);
}

void GUI_AbstractLibrary::delete_tracks(){

	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);
	SP::Set<int> indexes;

	for(const QModelIndex& idx : idx_list) {
		indexes.insert(idx.row());
	}

	Library::TrackDeletionMode answer = show_delete_dialog(indexes.size());

	if(answer != Library::TrackDeletionMode::None){
		_library->delete_tracks_by_idx(indexes, answer);
	}
}


void GUI_AbstractLibrary::album_rating_changed(int rating) {

	SP::Set<int> indexes = _lv_album->get_selections();
	if(indexes.isEmpty()) {
		return;
	}

	int first = indexes.first();

	_library->psl_album_rating_changed(first, rating);
}


void GUI_AbstractLibrary::title_rating_changed(int rating) {

	SP::Set<int> indexes = _lv_album->get_selections();
	if(indexes.isEmpty()) {
		return;
	}

	int first = indexes.first();

	_library->psl_track_rating_changed(first, rating);
}

void GUI_AbstractLibrary::append() {
	_library->psl_append_all_tracks();
}

void GUI_AbstractLibrary::append_tracks() {

	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);

	SP::Set<int> indexes;
	for(const QModelIndex&  idx : idx_list) {
		indexes.insert(idx.row());
	}

	_library->psl_append_tracks(indexes);
}


void GUI_AbstractLibrary::play_next() {

	_library->psl_play_next_all_tracks();
}


void GUI_AbstractLibrary::play_next_tracks() {

	QModelIndexList idx_list = _lv_tracks->selectionModel()->selectedRows(0);

	SP::Set<int> indexes;
	for(const QModelIndex&  idx : idx_list) {
		indexes.insert(idx.row());
	}

	_library->psl_play_next_tracks(indexes);
}

void GUI_AbstractLibrary::refresh_artist(){
	_library->refresh_artist();
}

void GUI_AbstractLibrary::refresh_album(){
	_library->refresh_albums();
}

void GUI_AbstractLibrary::refresh_tracks(){
	_library->refresh_tracks();
}


void GUI_AbstractLibrary::id3_tags_changed() {
	refresh();
}


void GUI_AbstractLibrary::show_delete_answer(QString answer) {
	Message::info(answer, tr("Library"));
}


void GUI_AbstractLibrary::_sl_live_search_changed(){

	if(_settings->get(Set::Lib_LiveSearch)){
		connect(_le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
		disconnect(_le_search, &QLineEdit::returnPressed, this, &GUI_AbstractLibrary::return_pressed);
	}

	else{
		disconnect(_le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
		connect(_le_search, &QLineEdit::returnPressed, this, &GUI_AbstractLibrary::return_pressed);
	}
}

void GUI_AbstractLibrary::set_index(int idx){

	if(!_combo_libchooser) {
		return;
	}

	_combo_libchooser->setCurrentIndex(idx);
}



