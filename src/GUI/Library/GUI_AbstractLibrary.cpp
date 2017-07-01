/* GUI_AbstractLibrary.cpp */

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

#include "GUI_AbstractLibrary.h"
#include "Views/LibraryView.h"
#include "Views/LibraryViewAlbum.h"
#include "Delegates/LibraryRatingDelegate.h"
#include "Models/LibraryItemModelAlbums.h"
#include "Models/LibraryItemModelArtists.h"
#include "Models/LibraryItemModelTracks.h"

#include "Components/Library/AbstractLibrary.h"

#include "Helper/Message/Message.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/Library/Filter.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"
#include "Helper/MetaData/MetaDataList.h"

#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Helper/ColumnIndex.h"
#include "GUI/Helper/Delegates/StyledItemDelegate.h"

#include <QKeySequence>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>

struct GUI_AbstractLibrary::Private
{
	AbstractLibrary* library = nullptr;
	::Library::Filter cur_searchfilter;
	BoolList shown_cols_albums;
	BoolList shown_cols_artist;
	BoolList shown_cols_tracks;

	LibraryTableView* lv_album=nullptr;
	LibraryTableView* lv_artist=nullptr;
	LibraryTableView* lv_tracks=nullptr;

	QComboBox* combo_search=nullptr;
	QPushButton* btn_clear=nullptr;
	QLineEdit* le_search=nullptr;


	LibraryItemModelTracks* track_model = nullptr;
	LibraryItemModelAlbums* album_model = nullptr;
	LibraryItemModelArtists* artist_model = nullptr;

	LibraryRatingDelegate* track_delegate = nullptr;
	LibraryRatingDelegate* album_delegate = nullptr;

	Private(AbstractLibrary* library) :
		library(library)
	{
		Settings* settings = Settings::getInstance();
		shown_cols_albums = settings->get(Set::Lib_ColsAlbum);
		shown_cols_artist = settings->get(Set::Lib_ColsArtist);
		shown_cols_tracks = settings->get(Set::Lib_ColsTitle);
	}
};

GUI_AbstractLibrary::GUI_AbstractLibrary(AbstractLibrary* library, QWidget *parent) :
	SayonaraWidget(parent)
{
	_m = Pimpl::make<Private>(library);
}

GUI_AbstractLibrary::~GUI_AbstractLibrary() {}

void GUI_AbstractLibrary::init()
{
	_m->lv_album = lv_album();
	_m->lv_artist = lv_artist();
	_m->lv_tracks = lv_tracks();

	_m->combo_search = combo_search();
	_m->btn_clear = btn_clear();
	_m->le_search = le_search();

	init_search_combobox();
	init_shortcuts();
	init_finished();
}

void GUI_AbstractLibrary::init_finished()
{
	init_headers();

	connect(_m->library, &AbstractLibrary::sig_all_artists_loaded, this, &GUI_AbstractLibrary::lib_fill_artists);
	connect(_m->library, &AbstractLibrary::sig_all_albums_loaded, this, &GUI_AbstractLibrary::lib_fill_albums);
	connect(_m->library, &AbstractLibrary::sig_all_tracks_loaded, this,	&GUI_AbstractLibrary::lib_fill_tracks);
	connect(_m->library, &AbstractLibrary::sig_track_mime_data_available, this, &GUI_AbstractLibrary::track_info_available);
	connect(_m->library, &AbstractLibrary::sig_delete_answer, this, &GUI_AbstractLibrary::show_delete_answer);

	connect(_m->lv_album, &LibraryViewAlbum::doubleClicked, this, &GUI_AbstractLibrary::album_dbl_clicked);
	connect(_m->lv_album, &LibraryViewAlbum::sig_sel_changed, this, &GUI_AbstractLibrary::album_sel_changed);
	connect(_m->lv_album, &LibraryViewAlbum::sig_middle_button_clicked, this, &GUI_AbstractLibrary::album_middle_clicked);
	connect(_m->lv_album, &LibraryViewAlbum::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_album_changed);
	connect(_m->lv_album, &LibraryViewAlbum::sig_columns_changed, this, &GUI_AbstractLibrary::columns_album_changed);
	connect(_m->lv_album, &LibraryViewAlbum::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_album);
	connect(_m->lv_album, &LibraryViewAlbum::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
	connect(_m->lv_album, &LibraryViewAlbum::sig_append_clicked, this, &GUI_AbstractLibrary::append);
	connect(_m->lv_album, &LibraryViewAlbum::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_album);

	connect(_m->lv_artist, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::artist_dbl_clicked);
	connect(_m->lv_artist, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::artist_sel_changed);
	connect(_m->lv_artist, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::artist_middle_clicked);
	connect(_m->lv_artist, &LibraryTableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_artist_changed);
	connect(_m->lv_artist, &LibraryTableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_artist_changed);
	connect(_m->lv_artist, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_artist);
	connect(_m->lv_artist, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
	connect(_m->lv_artist, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append);
	connect(_m->lv_artist, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_artist);

	connect(_m->lv_tracks, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::track_dbl_clicked);
	connect(_m->lv_tracks, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::track_sel_changed);
	connect(_m->lv_tracks, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::tracks_middle_clicked);
	connect(_m->lv_tracks, &LibraryTableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_title_changed);
	connect(_m->lv_tracks, &LibraryTableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_title_changed);
	connect(_m->lv_tracks, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_tracks);
	connect(_m->lv_tracks, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next_tracks);
	connect(_m->lv_tracks, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append_tracks);
	connect(_m->lv_tracks, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_tracks);

	connect(_m->btn_clear, &QPushButton::clicked, this, &GUI_AbstractLibrary::clear_button_pressed);

	// overloaded
	connect(_m->combo_search, combo_current_index_changed_int, this, &GUI_AbstractLibrary::combo_search_changed);

	REGISTER_LISTENER(Set::Lib_LiveSearch, _sl_live_search_changed);
}

void GUI_AbstractLibrary::init_search_combobox()
{
	QList<Library::Filter::Mode> filters = search_options();
	for(const Library::Filter::Mode filter : filters)
	{
		QVariant data = QVariant((int) (filter));
		_m->combo_search->addItem(::Library::Filter::get_text(filter), data);

	}
}

QList<Library::Filter::Mode> GUI_AbstractLibrary::search_options() const
{
	QList<Library::Filter::Mode> filters;

	filters << ::Library::Filter::Mode::Fulltext;
	filters << ::Library::Filter::Mode::Genre;
	filters << ::Library::Filter::Mode::Filename;

	return filters;
}

void GUI_AbstractLibrary::init_headers()
{
	Library::Sortings so = _settings->get(Set::Lib_Sorting);

	ColumnHeader* t_h0 = new ColumnHeader(ColumnHeader::Sharp, true, Library::SortOrder::TrackNumAsc, Library::SortOrder::TrackNumDesc, 25);
	ColumnHeader* t_h1 = new ColumnHeader(ColumnHeader::Title, false, Library::SortOrder::TrackTitleAsc, Library::SortOrder::TrackTitleDesc, 0.4, 200);
	ColumnHeader* t_h2 = new ColumnHeader(ColumnHeader::Artist, true, Library::SortOrder::TrackArtistAsc, Library::SortOrder::TrackArtistDesc, 0.3, 160);
	ColumnHeader* t_h3 = new ColumnHeader(ColumnHeader::Album, true, Library::SortOrder::TrackAlbumAsc, Library::SortOrder::TrackAlbumDesc, 0.3, 160);
	ColumnHeader* t_h4 = new ColumnHeader(ColumnHeader::Year, true, Library::SortOrder::TrackYearAsc, Library::SortOrder::TrackYearDesc, 50);
	ColumnHeader* t_h5 = new ColumnHeader(ColumnHeader::DurationShort, true, Library::SortOrder::TrackLenghtAsc, Library::SortOrder::TrackLengthDesc, 50);
	ColumnHeader* t_h6 = new ColumnHeader(ColumnHeader::Bitrate, true, Library::SortOrder::TrackBitrateAsc, Library::SortOrder::TrackBitrateDesc, 75);
	ColumnHeader* t_h7 = new ColumnHeader(ColumnHeader::Filesize, true, Library::SortOrder::TrackSizeAsc, Library::SortOrder::TrackSizeDesc, 75);
	ColumnHeader* t_h8 = new ColumnHeader(ColumnHeader::Rating, true, Library::SortOrder::TrackRatingAsc, Library::SortOrder::TrackRatingDesc, 80);

	ColumnHeader* al_h0 = new ColumnHeader(ColumnHeader::Sharp, true, Library::SortOrder::NoSorting, Library::SortOrder::NoSorting, 20);
	ColumnHeader* al_h1 = new ColumnHeader(ColumnHeader::Album, false, Library::SortOrder::AlbumNameAsc, Library::SortOrder::AlbumNameDesc, 1.0, 160);
	ColumnHeader* al_h2 = new ColumnHeader(ColumnHeader::Duration, true, Library::SortOrder::AlbumDurationAsc, Library::SortOrder::AlbumDurationDesc, 90);
	ColumnHeader* al_h3 = new ColumnHeader(ColumnHeader::NumTracks, true, Library::SortOrder::AlbumTracksAsc, Library::SortOrder::AlbumTracksDesc, 80);
	ColumnHeader* al_h4 = new ColumnHeader(ColumnHeader::Year, true, Library::SortOrder::AlbumYearAsc, Library::SortOrder::AlbumYearDesc, 50);
	ColumnHeader* al_h5 = new ColumnHeader(ColumnHeader::Rating, true, Library::SortOrder::AlbumRatingAsc, Library::SortOrder::AlbumRatingDesc, 80);

	ColumnHeader* ar_h0 = new ColumnHeader(ColumnHeader::Sharp, true, Library::SortOrder::NoSorting, Library::SortOrder::NoSorting, 20);
	ColumnHeader* ar_h1 = new ColumnHeader(ColumnHeader::Artist, false, Library::SortOrder::ArtistNameAsc, Library::SortOrder::ArtistNameDesc, 1.0, 160 );
	ColumnHeader* ar_h2 = new ColumnHeader(ColumnHeader::NumTracks, true, Library::SortOrder::ArtistTrackcountAsc, Library::SortOrder::ArtistTrackcountDesc, 80);

	ColumnHeaderList track_columns, album_columns, artist_columns;

	track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;
	album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4 << al_h5;
	artist_columns << ar_h0 << ar_h1 << ar_h2;

	_m->album_model = new LibraryItemModelAlbums(_m->lv_album);
	_m->artist_model = new LibraryItemModelArtists(_m->lv_artist);
	_m->track_model = new LibraryItemModelTracks(_m->lv_tracks);

	_m->album_delegate = new LibraryRatingDelegate(_m->lv_album, (int) ColumnIndex::Album::Rating, true);
	_m->track_delegate = new LibraryRatingDelegate(_m->lv_tracks, (int) ColumnIndex::Track::Rating, true);

	connect(_m->album_delegate, &LibraryRatingDelegate::sig_rating_changed, this, &GUI_AbstractLibrary::album_rating_changed);
	connect(_m->track_delegate, &LibraryRatingDelegate::sig_rating_changed, this, &GUI_AbstractLibrary::title_rating_changed);

	_m->lv_tracks->setModel(_m->track_model);
	_m->lv_tracks->setSearchModel(_m->track_model);
	_m->lv_tracks->setItemDelegate(_m->track_delegate);
	_m->lv_tracks->set_metadata_interpretation(MD::Interpretation::Tracks);
	_m->lv_tracks->set_table_headers(track_columns, _m->shown_cols_tracks, so.so_tracks);

	_m->lv_artist->setModel(_m->artist_model);
	_m->lv_artist->setSearchModel(_m->artist_model);
	_m->lv_artist->setItemDelegate(new StyledItemDelegate(_m->lv_artist));
	_m->lv_artist->set_metadata_interpretation(MD::Interpretation::Artists);
	_m->lv_artist->set_table_headers(artist_columns, _m->shown_cols_artist, so.so_artists);

	_m->lv_album->setModel(_m->album_model);
	_m->lv_album->setSearchModel(_m->album_model);
	_m->lv_album->setItemDelegate(_m->album_delegate);
	_m->lv_album->set_metadata_interpretation(MD::Interpretation::Albums);
	_m->lv_album->set_table_headers(album_columns, _m->shown_cols_albums, so.so_albums);
}

void GUI_AbstractLibrary::init_shortcuts() {}


void GUI_AbstractLibrary::text_line_edited(const QString &search)
{
	if(search.startsWith("f:", Qt::CaseInsensitive)) {
		_m->combo_search->setCurrentIndex(0);
		_m->le_search->clear();
	}

	else if(search.startsWith("g:", Qt::CaseInsensitive)) {
		_m->combo_search->setCurrentIndex(1);
		_m->le_search->clear();
	}

	else if(search.startsWith("p:", Qt::CaseInsensitive)) {
		_m->combo_search->setCurrentIndex(2);
		_m->le_search->clear();
	}

	Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);
	Library::Filter filter;

	QString text = search;

	Library::Filter::Mode current_mode =
			(Library::Filter::Mode) (_m->combo_search->currentData().toInt());

	switch(current_mode)
	{
		case Library::Filter::Fulltext:
			text = Library::convert_search_string(search, mask);
			break;

		default:
			break;
	}

	filter.set_mode(current_mode);

	if(search.size() < 3){
		filter.clear();
	}

	else{
		filter.set_filtertext( QString("%") + text + QString("%") );
	}

	_m->cur_searchfilter = filter;
	_m->library->psl_filter_changed(filter);
}


void GUI_AbstractLibrary::clear_button_pressed()
{
	disconnect(_m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);

	_m->combo_search->setCurrentIndex(0);
	_m->le_search->clear();

	_m->library->refetch();

	if(_settings->get(Set::Lib_LiveSearch)){
		connect(_m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
	}
}


void GUI_AbstractLibrary::combo_search_changed(int idx)
{
	Q_UNUSED(idx)

	int i_current_data = _m->combo_search->currentData().toInt();
	Library::Filter::Mode mode = static_cast<Library::Filter::Mode>(i_current_data);

	_m->cur_searchfilter.set_mode(mode);
	_m->library->psl_filter_changed(_m->cur_searchfilter);
}


void GUI_AbstractLibrary::return_pressed()
{
	text_line_edited(_m->le_search->text());
}


void GUI_AbstractLibrary::refresh()
{
	_m->library->refresh();
}


void GUI_AbstractLibrary::lib_fill_tracks(const MetaDataList& v_md)
{
	_m->lv_tracks->fill<MetaDataList, LibraryItemModelTracks>(v_md);
	_m->artist_model->set_mimedata(v_md);
	_m->album_model->set_mimedata(v_md);
}


void GUI_AbstractLibrary::lib_fill_albums(const AlbumList& albums)
{
	_m->lv_album->fill<AlbumList, LibraryItemModelAlbums>(albums);
}


void GUI_AbstractLibrary::lib_fill_artists(const ArtistList& artists)
{
	_m->lv_artist->fill<ArtistList, LibraryItemModelArtists>(artists);
}


void GUI_AbstractLibrary::track_info_available(const MetaDataList& v_md)
{
	_m->track_model->set_mimedata(v_md);
}


void GUI_AbstractLibrary::artist_sel_changed(const SP::Set<int>& lst)
{
	_m->library->psl_selected_artists_changed(lst);
}


void GUI_AbstractLibrary::album_sel_changed(const SP::Set<int>& lst)
{
	_m->library->psl_selected_albums_changed(lst);
}

void GUI_AbstractLibrary::track_sel_changed(const SP::Set<int>& lst)
{
	_m->library->psl_selected_tracks_changed(lst);
}


void GUI_AbstractLibrary::artist_middle_clicked(const QPoint& pt)
{
	Q_UNUSED(pt)
	_m->library->psl_prepare_tracks_for_playlist(true);
}


void GUI_AbstractLibrary::album_middle_clicked(const QPoint& pt)
{
	Q_UNUSED(pt)
	_m->library->psl_prepare_tracks_for_playlist(true);
}

void GUI_AbstractLibrary::tracks_middle_clicked(const QPoint& pt)
{
	Q_UNUSED(pt)
	_m->library->psl_prepare_tracks_for_playlist(_m->lv_tracks->get_selected_items(), true);
}


void GUI_AbstractLibrary::album_dbl_clicked(const QModelIndex& idx)
{
	LibraryView* view = static_cast<LibraryView*>( sender() );

	_m->library->psl_prepare_album_for_playlist(
				view->get_index_by_model_index(idx),
				false
	);
}

void GUI_AbstractLibrary::artist_dbl_clicked(const QModelIndex& idx)
{
	LibraryView* view = static_cast<LibraryView*>( sender() );
	_m->library->psl_prepare_artist_for_playlist(
				view->get_index_by_model_index(idx),
				false
	);
}

void GUI_AbstractLibrary::track_dbl_clicked(const QModelIndex& idx)
{
	LibraryView* view = static_cast<LibraryView*>( sender() );

	_m->library->psl_prepare_tracks_for_playlist(
				view->get_index_by_model_index(idx),
				false
	);
}


void  GUI_AbstractLibrary::columns_album_changed(const BoolList& list)
{
	_m->shown_cols_albums = list;
	_settings->set(Set::Lib_ColsAlbum, list);
}


void  GUI_AbstractLibrary::columns_artist_changed(const BoolList& list)
{
	_m->shown_cols_artist = list;
	_settings->set(Set::Lib_ColsArtist, list);
}


void  GUI_AbstractLibrary::columns_title_changed(const BoolList& list)
{
	_m->shown_cols_tracks = list;
	_settings->set(Set::Lib_ColsTitle, list);
}


void GUI_AbstractLibrary::sortorder_artist_changed(Library::SortOrder s)
{
	_m->lv_artist->save_selections();

	Library::Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_artists = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_album_changed(Library::SortOrder s)
{
	_m->lv_album->save_selections();

   Library::Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_albums = s;

	_settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_title_changed(Library::SortOrder s)
{
	_m->lv_tracks->save_selections();

	Library::Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_tracks = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::delete_album()
{
	int n_tracks = _m->track_model->rowCount();
	Library::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_m->library->delete_current_tracks(answer);
}

void GUI_AbstractLibrary::delete_artist()
{
	int n_tracks = _m->track_model->rowCount();
	Library::TrackDeletionMode answer = show_delete_dialog(n_tracks);

	_m->library->delete_current_tracks(answer);
}

void GUI_AbstractLibrary::delete_tracks()
{
	QModelIndexList idx_list = _m->lv_tracks->selectionModel()->selectedRows(0);
	SP::Set<int> indexes;

	for(const QModelIndex& idx : idx_list) {
		indexes.insert(idx.row());
	}

	Library::TrackDeletionMode answer = show_delete_dialog(indexes.size());

	if(answer != Library::TrackDeletionMode::None){
		_m->library->delete_tracks_by_idx(indexes, answer);
	}
}


void GUI_AbstractLibrary::album_rating_changed(int rating)
{
	SP::Set<int> indexes = _m->lv_album->get_selected_items();
	if(indexes.isEmpty()) {
		return;
	}

	int first = indexes.first();

	_m->library->change_album_rating(first, rating);
}


void GUI_AbstractLibrary::title_rating_changed(int rating)
{
	SP::Set<int> indexes = _m->lv_tracks->get_selected_items();
	if(indexes.isEmpty()) {
		return;
	}

	int first = indexes.first();

	_m->library->change_track_rating(first, rating);
}

void GUI_AbstractLibrary::append()
{
	_m->library->psl_append_all_tracks();
}

void GUI_AbstractLibrary::append_tracks()
{
	QModelIndexList idx_list = _m->lv_tracks->selectionModel()->selectedRows(0);

	SP::Set<int> indexes;
	for(const QModelIndex&  idx : idx_list) {
		indexes.insert(idx.row());
	}

	_m->library->psl_append_tracks(indexes);
}


void GUI_AbstractLibrary::play_next()
{
	_m->library->psl_play_next_all_tracks();
}


void GUI_AbstractLibrary::play_next_tracks()
{
	QModelIndexList idx_list = _m->lv_tracks->selectionModel()->selectedRows(0);

	SP::Set<int> indexes;
	for(const QModelIndex&  idx : idx_list) {
		indexes.insert(idx.row());
	}

	_m->library->psl_play_next_tracks(indexes);
}

void GUI_AbstractLibrary::refresh_artist()
{
	_m->library->refresh_artist();
}

void GUI_AbstractLibrary::refresh_album()
{
	_m->library->refresh_albums();
}

void GUI_AbstractLibrary::refresh_tracks()
{
	_m->library->refresh_tracks();
}


void GUI_AbstractLibrary::id3_tags_changed()
{
	refresh();
}


void GUI_AbstractLibrary::show_delete_answer(QString answer)
{
	Message::info(answer, Lang::get(Lang::Library));
}


void GUI_AbstractLibrary::_sl_live_search_changed()
{
	if(_settings->get(Set::Lib_LiveSearch)){
		connect(_m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
		disconnect(_m->le_search, &QLineEdit::returnPressed, this, &GUI_AbstractLibrary::return_pressed);
	}

	else{
		disconnect(_m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::text_line_edited);
		connect(_m->le_search, &QLineEdit::returnPressed, this, &GUI_AbstractLibrary::return_pressed);
	}
}
