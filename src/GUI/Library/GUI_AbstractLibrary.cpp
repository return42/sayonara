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
#include "Views/View.h"
#include "Views/AlbumView.h"
#include "Delegates/RatingDelegate.h"
#include "Models/AlbumModel.h"
#include "Models/ArtistModel.h"
#include "Models/TrackModel.h"

#include "Components/Library/AbstractLibrary.h"

#include "Utils/Message/Message.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/Library/Filter.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Logger/Logger.h"

#include "GUI/Library/Utils/ColumnHeader.h"
#include "GUI/Library/Utils/ColumnIndex.h"
#include "GUI/Utils/Delegates/StyledItemDelegate.h"
#include "GUI/Utils/EventFilter.h"

#include <QLineEdit>
#include <QMenu>

using namespace Library;

struct GUI_AbstractLibrary::Private
{
	AbstractLibrary* library = nullptr;

	TableView* lv_album=nullptr;
	TableView* lv_artist=nullptr;
	TableView* lv_tracks=nullptr;

	QLineEdit* le_search=nullptr;

	Private(AbstractLibrary* library) :
		library(library)
    {}
};

GUI_AbstractLibrary::GUI_AbstractLibrary(AbstractLibrary* library, QWidget *parent) :
	Widget(parent)
{
	m = Pimpl::make<Private>(library);
}

GUI_AbstractLibrary::~GUI_AbstractLibrary() {}

void GUI_AbstractLibrary::init()
{
	m->lv_album = lv_album();
	m->lv_artist = lv_artist();
	m->lv_tracks = lv_tracks();
	m->le_search = le_search();

    Set::listen(Set::Lib_UseViewClearButton, [&]()
    {
        bool use_clear_button = _settings->get(Set::Lib_UseViewClearButton);
        m->lv_album->use_clear_button(use_clear_button);
        m->lv_artist->use_clear_button(use_clear_button);

    });


    KeyPressFilter* kp_filter = new KeyPressFilter(m->le_search);
    this->installEventFilter(kp_filter);

    connect(kp_filter, &KeyPressFilter::sig_esc_pressed, this, &GUI_AbstractLibrary::search_cleared);

    init_views();
    init_headers();
    init_search_bar();
	init_shortcuts();
    init_connections();
}


void GUI_AbstractLibrary::init_views()
{
	TrackModel* track_model = new TrackModel(m->lv_tracks, m->library);
    RatingDelegate* track_delegate = new RatingDelegate(m->lv_tracks, (int) ColumnIndex::Track::Rating, true);

    m->lv_tracks->setModel(track_model);
    m->lv_tracks->setSearchModel(track_model);
    m->lv_tracks->setItemDelegate(track_delegate);
    m->lv_tracks->set_metadata_interpretation(MD::Interpretation::Tracks);

	ArtistModel* artist_model = new ArtistModel(m->lv_artist, m->library);
    m->lv_artist->setModel(artist_model);
    m->lv_artist->setSearchModel(artist_model);
	m->lv_artist->setItemDelegate(new Gui::StyledItemDelegate(m->lv_artist));
    m->lv_artist->set_metadata_interpretation(MD::Interpretation::Artists);

	AlbumModel* album_model = new AlbumModel(m->lv_album, m->library);
    RatingDelegate* album_delegate = new RatingDelegate(m->lv_album, (int) ColumnIndex::Album::Rating, true);

    m->lv_album->setModel(album_model);
    m->lv_album->setSearchModel(album_model);
    m->lv_album->setItemDelegate(album_delegate);
    m->lv_album->set_metadata_interpretation(MD::Interpretation::Albums);
}

void GUI_AbstractLibrary::init_headers()
{
	Sortings so = _settings->get(Set::Lib_Sorting);

	ColumnHeader* t_h0 = new ColumnHeader(ColumnHeader::Sharp, true, SortOrder::TrackNumAsc, SortOrder::TrackNumDesc, 25);
	ColumnHeader* t_h1 = new ColumnHeader(ColumnHeader::Title, false, SortOrder::TrackTitleAsc, SortOrder::TrackTitleDesc, 0.4, 200);
	ColumnHeader* t_h2 = new ColumnHeader(ColumnHeader::Artist, true, SortOrder::TrackArtistAsc, SortOrder::TrackArtistDesc, 0.3, 160);
	ColumnHeader* t_h3 = new ColumnHeader(ColumnHeader::Album, true, SortOrder::TrackAlbumAsc, SortOrder::TrackAlbumDesc, 0.3, 160);
	ColumnHeader* t_h4 = new ColumnHeader(ColumnHeader::Year, true, SortOrder::TrackYearAsc, SortOrder::TrackYearDesc, 50);
	ColumnHeader* t_h5 = new ColumnHeader(ColumnHeader::DurationShort, true, SortOrder::TrackLenghtAsc, SortOrder::TrackLengthDesc, 50);
	ColumnHeader* t_h6 = new ColumnHeader(ColumnHeader::Bitrate, true, SortOrder::TrackBitrateAsc, SortOrder::TrackBitrateDesc, 75);
	ColumnHeader* t_h7 = new ColumnHeader(ColumnHeader::Filesize, true, SortOrder::TrackSizeAsc, SortOrder::TrackSizeDesc, 75);
	ColumnHeader* t_h8 = new ColumnHeader(ColumnHeader::Rating, true, SortOrder::TrackRatingAsc, SortOrder::TrackRatingDesc, 80);

	ColumnHeader* al_h0 = new ColumnHeader(ColumnHeader::Sharp, true, SortOrder::NoSorting, SortOrder::NoSorting, 20);
	ColumnHeader* al_h1 = new ColumnHeader(ColumnHeader::Album, false, SortOrder::AlbumNameAsc, SortOrder::AlbumNameDesc, 1.0, 160);
	ColumnHeader* al_h2 = new ColumnHeader(ColumnHeader::Duration, true, SortOrder::AlbumDurationAsc, SortOrder::AlbumDurationDesc, 90);
	ColumnHeader* al_h3 = new ColumnHeader(ColumnHeader::NumTracks, true, SortOrder::AlbumTracksAsc, SortOrder::AlbumTracksDesc, 80);
	ColumnHeader* al_h4 = new ColumnHeader(ColumnHeader::Year, true, SortOrder::AlbumYearAsc, SortOrder::AlbumYearDesc, 50);
	ColumnHeader* al_h5 = new ColumnHeader(ColumnHeader::Rating, true, SortOrder::AlbumRatingAsc, SortOrder::AlbumRatingDesc, 80);

	ColumnHeader* ar_h0 = new ColumnHeader(ColumnHeader::Sharp, true, SortOrder::NoSorting, SortOrder::NoSorting, 20);
	ColumnHeader* ar_h1 = new ColumnHeader(ColumnHeader::Artist, false, SortOrder::ArtistNameAsc, SortOrder::ArtistNameDesc, 1.0, 160 );
	ColumnHeader* ar_h2 = new ColumnHeader(ColumnHeader::NumTracks, true, SortOrder::ArtistTrackcountAsc, SortOrder::ArtistTrackcountDesc, 80);

    ColumnHeaderList track_columns, album_columns, artist_columns;

    track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;
    album_columns  << al_h0 << al_h1 << al_h2 << al_h3 << al_h4 << al_h5;
    artist_columns << ar_h0 << ar_h1 << ar_h2;

    BoolList shown_cols_albums = _settings->get(Set::Lib_ColsAlbum);
    BoolList shown_cols_artist = _settings->get(Set::Lib_ColsArtist);
    BoolList shown_cols_tracks = _settings->get(Set::Lib_ColsTitle);

    m->lv_tracks->set_table_headers(track_columns, shown_cols_tracks, so.so_tracks);
    m->lv_artist->set_table_headers(artist_columns, shown_cols_artist, so.so_artists);
    m->lv_album->set_table_headers(album_columns, shown_cols_albums, so.so_albums);
}


void GUI_AbstractLibrary::init_search_bar()
{
    m->le_search->setContextMenuPolicy(Qt::CustomContextMenu);
    m->le_search->setClearButtonEnabled(true);


    QList<QAction*> actions;
	QList<Filter::Mode> filters = search_options();
	for(const Filter::Mode filter_mode : filters)
    {
        QVariant data = QVariant((int) (filter_mode));
		QAction* action = new QAction(Filter::get_text(filter_mode), m->le_search);

        action->setCheckable(false);
        action->setData(data);

        actions << action;

		connect(action, &QAction::triggered, [=](){
            search_mode_changed(filter_mode);
            m->library->refetch();
        });
    }

    QMenu* menu = new QMenu(m->le_search);
    menu->addActions(actions);

    ContextMenuFilter* cm_filter = new ContextMenuFilter(m->le_search);
    connect(cm_filter, &ContextMenuFilter::sig_context_menu, menu, &QMenu::popup);

    m->le_search->installEventFilter(cm_filter);
    connect(m->le_search, &QLineEdit::returnPressed, this, &GUI_AbstractLibrary::search_return_pressed);

	search_mode_changed(Filter::Fulltext);
}

void GUI_AbstractLibrary::init_connections()
{
    connect(m->library, &AbstractLibrary::sig_all_artists_loaded, this, &GUI_AbstractLibrary::lib_artists_ready);
    connect(m->library, &AbstractLibrary::sig_all_albums_loaded, this, &GUI_AbstractLibrary::lib_albums_ready);
    connect(m->library, &AbstractLibrary::sig_all_tracks_loaded, this,	&GUI_AbstractLibrary::lib_tracks_ready);
    connect(m->library, &AbstractLibrary::sig_delete_answer, this, &GUI_AbstractLibrary::show_delete_answer);

    connect(m->lv_album, &AlbumView::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
    connect(m->lv_album, &AlbumView::sig_sel_changed, this, &GUI_AbstractLibrary::album_sel_changed);
    connect(m->lv_album, &AlbumView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
    connect(m->lv_album, &AlbumView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_album_changed);
    connect(m->lv_album, &AlbumView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_album_changed);
    connect(m->lv_album, &AlbumView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
    connect(m->lv_album, &AlbumView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
    connect(m->lv_album, &AlbumView::sig_append_clicked, this, &GUI_AbstractLibrary::append);
    connect(m->lv_album, &AlbumView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_album);

	connect(m->lv_artist, &View::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
	connect(m->lv_artist, &View::sig_sel_changed, this, &GUI_AbstractLibrary::artist_sel_changed);
	connect(m->lv_artist, &View::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
	connect(m->lv_artist, &TableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_artist_changed);
	connect(m->lv_artist, &TableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_artist_changed);
	connect(m->lv_artist, &View::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
	connect(m->lv_artist, &View::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
	connect(m->lv_artist, &View::sig_append_clicked, this, &GUI_AbstractLibrary::append);
	connect(m->lv_artist, &View::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_artist);

	connect(m->lv_tracks, &View::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
	connect(m->lv_tracks, &View::sig_sel_changed, this, &GUI_AbstractLibrary::track_sel_changed);
	connect(m->lv_tracks, &View::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
	connect(m->lv_tracks, &TableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_title_changed);
	connect(m->lv_tracks, &TableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_title_changed);
	connect(m->lv_tracks, &View::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
	connect(m->lv_tracks, &View::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next_tracks);
	connect(m->lv_tracks, &View::sig_append_clicked, this, &GUI_AbstractLibrary::append_tracks);
	connect(m->lv_tracks, &View::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_tracks);

	Set::listen(Set::Lib_LiveSearch, this, &GUI_AbstractLibrary::_sl_live_search_changed);
}

void GUI_AbstractLibrary::init_shortcuts() {}

void GUI_AbstractLibrary::search_return_pressed()
{
    search_edited(m->le_search->text());
}

void GUI_AbstractLibrary::search_edited(const QString& search)
{
    if(search.startsWith("f:", Qt::CaseInsensitive))
    {
        search_mode_changed(::Library::Filter::Fulltext);
		m->le_search->clear();
	}

	else if(search.startsWith("g:", Qt::CaseInsensitive)) {
        search_mode_changed(::Library::Filter::Genre);
		m->le_search->clear();
	}

	else if(search.startsWith("p:", Qt::CaseInsensitive)) {
        search_mode_changed(::Library::Filter::Filename);
		m->le_search->clear();
	}

	SearchModeMask mask = _settings->get(Set::Lib_SearchMode);
	Filter filter;
	Filter::Mode current_mode = static_cast<Filter::Mode>(m->le_search->property("search_mode").toInt());
    filter.set_mode(current_mode);

	QString text = search;

	if(current_mode == Filter::Fulltext){
		text = Util::convert_search_string(search, mask);
    }

    if(search.size() < 3) {
		filter.clear();
	}

    else {
		filter.set_filtertext( QString("%") + text + QString("%") );
	}

    m->library->filter_changed(filter);
}

void GUI_AbstractLibrary::search_cleared()
{
	Filter filter;
    m->library->set_filter(filter);

	search_mode_changed(Filter::Fulltext);

    m->le_search->clear();
    m->library->refetch();
}


void GUI_AbstractLibrary::search_mode_changed(Filter::Mode mode)
{
	QString text = Lang::get(Lang::Search) + ": " + Filter::get_text(mode);

    m->le_search->setPlaceholderText(text);
    m->le_search->setProperty("search_mode", (int) mode);

	Filter filter = m->library->filter();
    filter.set_mode(mode);

    m->library->set_filter(filter);
}

void GUI_AbstractLibrary::lib_tracks_ready()
{
    const MetaDataList& v_md = m->library->tracks();

	m->lv_tracks->fill<MetaDataList, TrackModel>(v_md);
}

void GUI_AbstractLibrary::lib_albums_ready()
{
    const AlbumList& albums = m->library->albums();

	m->lv_album->fill<AlbumList, AlbumModel>(albums);
}


void GUI_AbstractLibrary::lib_artists_ready()
{
    const ArtistList& artists = m->library->artists();

	m->lv_artist->fill<ArtistList, ArtistModel>(artists);
}


void GUI_AbstractLibrary::artist_sel_changed(const IndexSet& lst)
{
	m->library->selected_artists_changed(lst);
}


void GUI_AbstractLibrary::album_sel_changed(const IndexSet& lst)
{
	m->library->selected_albums_changed(lst);
}

void GUI_AbstractLibrary::track_sel_changed(const IndexSet& lst)
{
	m->library->selected_tracks_changed(lst);
}


void GUI_AbstractLibrary::item_middle_clicked(const QPoint& pt)
{
	Q_UNUSED(pt)
	m->library->prepare_tracks_for_playlist(true);
}

void GUI_AbstractLibrary::item_double_clicked(const QModelIndex& idx)
{
    Q_UNUSED(idx)
    m->library->prepare_tracks_for_playlist(false);
}

void  GUI_AbstractLibrary::columns_album_changed()
{
    _settings->set(Set::Lib_ColsAlbum, m->lv_album->get_shown_columns());
}


void  GUI_AbstractLibrary::columns_artist_changed()
{
    _settings->set(Set::Lib_ColsArtist, m->lv_artist->get_shown_columns());
}


void  GUI_AbstractLibrary::columns_title_changed()
{
    _settings->set(Set::Lib_ColsTitle, m->lv_tracks->get_shown_columns());
}


void GUI_AbstractLibrary::sortorder_artist_changed(SortOrder s)
{
	Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_artists = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_album_changed(SortOrder s)
{
	Sortings so = _settings->get(Set::Lib_Sorting);
    so.so_albums = s;

	_settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_title_changed(SortOrder s)
{
	Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_tracks = s;

    _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::delete_current_tracks()
{
    int n_tracks = m->library->current_tracks().count();

	TrackDeletionMode answer = show_delete_dialog(n_tracks);
	if(answer != TrackDeletionMode::None) {
        m->library->delete_current_tracks(answer);
    }
}

void GUI_AbstractLibrary::append()
{
	m->library->append_all_tracks();
}

void GUI_AbstractLibrary::append_tracks()
{
    m->library->append_current_tracks();
}

void GUI_AbstractLibrary::play_next()
{
	m->library->play_next_all_tracks();
}

void GUI_AbstractLibrary::play_next_tracks()
{
    m->library->play_next_current_tracks();
}

void GUI_AbstractLibrary::refresh_artist()
{
	m->library->refresh_artist();
}

void GUI_AbstractLibrary::refresh_album()
{
	m->library->refresh_albums();
}

void GUI_AbstractLibrary::refresh_tracks()
{
	m->library->refresh_tracks();
}


void GUI_AbstractLibrary::id3_tags_changed()
{
    m->library->refresh();
}


void GUI_AbstractLibrary::show_delete_answer(QString answer)
{
	Message::info(answer, Lang::get(Lang::Library));
}


void GUI_AbstractLibrary::_sl_live_search_changed()
{
    if(_settings->get(Set::Lib_LiveSearch)) {
        connect(m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::search_edited);
	}

    else {
        disconnect(m->le_search, &QLineEdit::textEdited, this, &GUI_AbstractLibrary::search_edited);
	}
}
