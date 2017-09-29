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
#include "Helper/Logger/Logger.h"

#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Helper/ColumnIndex.h"
#include "GUI/Helper/Delegates/StyledItemDelegate.h"
#include "GUI/Helper/EventFilter.h"

#include <QLineEdit>

struct GUI_AbstractLibrary::Private
{
	AbstractLibrary* library = nullptr;

	BoolList shown_cols_albums;
	BoolList shown_cols_artist;
	BoolList shown_cols_tracks;

	LibraryTableView* lv_album=nullptr;
	LibraryTableView* lv_artist=nullptr;
	LibraryTableView* lv_tracks=nullptr;

	QLineEdit* le_search=nullptr;

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
	m = Pimpl::make<Private>(library);
}

GUI_AbstractLibrary::~GUI_AbstractLibrary() {}

void GUI_AbstractLibrary::init()
{
	m->lv_album = lv_album();
	m->lv_artist = lv_artist();
	m->lv_tracks = lv_tracks();
	m->le_search = le_search();

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
    LibraryItemModelTracks* track_model = new LibraryItemModelTracks(m->lv_tracks, m->library);
    LibraryRatingDelegate* track_delegate = new LibraryRatingDelegate(m->lv_tracks, (int) ColumnIndex::Track::Rating, true);

    m->lv_tracks->setModel(track_model);
    m->lv_tracks->setSearchModel(track_model);
    m->lv_tracks->setItemDelegate(track_delegate);
    m->lv_tracks->set_metadata_interpretation(MD::Interpretation::Tracks);

    LibraryItemModelArtists* artist_model = new LibraryItemModelArtists(m->lv_artist, m->library);
    m->lv_artist->setModel(artist_model);
    m->lv_artist->setSearchModel(artist_model);
    m->lv_artist->setItemDelegate(new StyledItemDelegate(m->lv_artist));
    m->lv_artist->set_metadata_interpretation(MD::Interpretation::Artists);

    LibraryItemModelAlbums* album_model = new LibraryItemModelAlbums(m->lv_album, m->library);
    LibraryRatingDelegate* album_delegate = new LibraryRatingDelegate(m->lv_album, (int) ColumnIndex::Album::Rating, true);

    m->lv_album->setModel(album_model);
    m->lv_album->setSearchModel(album_model);
    m->lv_album->setItemDelegate(album_delegate);
    m->lv_album->set_metadata_interpretation(MD::Interpretation::Albums);
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

    m->lv_tracks->set_table_headers(track_columns, m->shown_cols_tracks, so.so_tracks);
    m->lv_artist->set_table_headers(artist_columns, m->shown_cols_artist, so.so_artists);
    m->lv_album->set_table_headers(album_columns, m->shown_cols_albums, so.so_albums);
}


void GUI_AbstractLibrary::init_search_bar()
{
    m->le_search->setContextMenuPolicy(Qt::CustomContextMenu);
    m->le_search->setClearButtonEnabled(true);


    QList<QAction*> actions;
    QList<Library::Filter::Mode> filters = search_options();
    for(const Library::Filter::Mode filter_mode : filters)
    {
        QVariant data = QVariant((int) (filter_mode));
        QAction* action = new QAction(::Library::Filter::get_text(filter_mode), m->le_search);

        action->setCheckable(false);
        action->setData(data);

        actions << action;

        connect(action, &QAction::triggered, this, [=](){
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

    search_mode_changed(::Library::Filter::Fulltext);
}

void GUI_AbstractLibrary::init_connections()
{
    connect(m->library, &AbstractLibrary::sig_all_artists_loaded, this, &GUI_AbstractLibrary::lib_artists_ready);
    connect(m->library, &AbstractLibrary::sig_all_albums_loaded, this, &GUI_AbstractLibrary::lib_albums_ready);
    connect(m->library, &AbstractLibrary::sig_all_tracks_loaded, this,	&GUI_AbstractLibrary::lib_tracks_ready);
    connect(m->library, &AbstractLibrary::sig_delete_answer, this, &GUI_AbstractLibrary::show_delete_answer);

    connect(m->lv_album, &LibraryViewAlbum::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
    connect(m->lv_album, &LibraryViewAlbum::sig_sel_changed, this, &GUI_AbstractLibrary::album_sel_changed);
    connect(m->lv_album, &LibraryViewAlbum::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
    connect(m->lv_album, &LibraryViewAlbum::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_album_changed);
    connect(m->lv_album, &LibraryViewAlbum::sig_columns_changed, this, &GUI_AbstractLibrary::columns_album_changed);
    connect(m->lv_album, &LibraryViewAlbum::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
    connect(m->lv_album, &LibraryViewAlbum::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
    connect(m->lv_album, &LibraryViewAlbum::sig_append_clicked, this, &GUI_AbstractLibrary::append);
    connect(m->lv_album, &LibraryViewAlbum::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_album);

    connect(m->lv_artist, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
    connect(m->lv_artist, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::artist_sel_changed);
    connect(m->lv_artist, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
    connect(m->lv_artist, &LibraryTableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_artist_changed);
    connect(m->lv_artist, &LibraryTableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_artist_changed);
    connect(m->lv_artist, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
    connect(m->lv_artist, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next);
    connect(m->lv_artist, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append);
    connect(m->lv_artist, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_artist);

    connect(m->lv_tracks, &LibraryView::doubleClicked, this, &GUI_AbstractLibrary::item_double_clicked);
    connect(m->lv_tracks, &LibraryView::sig_sel_changed, this, &GUI_AbstractLibrary::track_sel_changed);
    connect(m->lv_tracks, &LibraryView::sig_middle_button_clicked, this, &GUI_AbstractLibrary::item_middle_clicked);
    connect(m->lv_tracks, &LibraryTableView::sig_sortorder_changed, this, &GUI_AbstractLibrary::sortorder_title_changed);
    connect(m->lv_tracks, &LibraryTableView::sig_columns_changed, this, &GUI_AbstractLibrary::columns_title_changed);
    connect(m->lv_tracks, &LibraryView::sig_delete_clicked, this, &GUI_AbstractLibrary::delete_current_tracks);
    connect(m->lv_tracks, &LibraryView::sig_play_next_clicked, this, &GUI_AbstractLibrary::play_next_tracks);
    connect(m->lv_tracks, &LibraryView::sig_append_clicked, this, &GUI_AbstractLibrary::append_tracks);
    connect(m->lv_tracks, &LibraryView::sig_refresh_clicked, this, &GUI_AbstractLibrary::refresh_tracks);

    REGISTER_LISTENER(Set::Lib_LiveSearch, _sl_live_search_changed);
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

	Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);
	Library::Filter filter;
    Library::Filter::Mode current_mode = static_cast<Library::Filter::Mode>(m->le_search->property("search_mode").toInt());
    filter.set_mode(current_mode);

	QString text = search;

    if(current_mode == Library::Filter::Fulltext){
        text = Library::convert_search_string(search, mask);
    }

    if(search.size() < 3) {
		filter.clear();
	}

    else {
		filter.set_filtertext( QString("%") + text + QString("%") );
	}

    m->library->psl_filter_changed(filter);
}

void GUI_AbstractLibrary::search_cleared()
{
    Library::Filter filter;
    m->library->set_filter(filter);

    search_mode_changed(Library::Filter::Fulltext);

    m->le_search->clear();
    m->library->refetch();
}


void GUI_AbstractLibrary::search_mode_changed(Library::Filter::Mode mode)
{
    QString text = Lang::get(Lang::Search) + ": " + Library::Filter::get_text(mode);

    m->le_search->setPlaceholderText(text);
    m->le_search->setProperty("search_mode", (int) mode);

    Library::Filter filter = m->library->get_filter();
    filter.set_mode(mode);

    m->library->set_filter(filter);
}

void GUI_AbstractLibrary::lib_tracks_ready()
{
    const MetaDataList& v_md = m->library->get_tracks();

    m->lv_tracks->fill<MetaDataList, LibraryItemModelTracks>(v_md);
}

void GUI_AbstractLibrary::lib_albums_ready()
{
    const AlbumList& albums = m->library->get_albums();

	m->lv_album->fill<AlbumList, LibraryItemModelAlbums>(albums);
}


void GUI_AbstractLibrary::lib_artists_ready()
{
    const ArtistList& artists = m->library->get_artists();

	m->lv_artist->fill<ArtistList, LibraryItemModelArtists>(artists);
}


void GUI_AbstractLibrary::artist_sel_changed(const SP::Set<int>& lst)
{
	m->library->psl_selected_artists_changed(lst);
}


void GUI_AbstractLibrary::album_sel_changed(const SP::Set<int>& lst)
{
	m->library->psl_selected_albums_changed(lst);
}

void GUI_AbstractLibrary::track_sel_changed(const SP::Set<int>& lst)
{
	m->library->psl_selected_tracks_changed(lst);
}


void GUI_AbstractLibrary::item_middle_clicked(const QPoint& pt)
{
	Q_UNUSED(pt)
	m->library->psl_prepare_tracks_for_playlist(true);
}

void GUI_AbstractLibrary::item_double_clicked(const QModelIndex& idx)
{
    Q_UNUSED(idx)
    m->library->psl_prepare_tracks_for_playlist(false);
}

void  GUI_AbstractLibrary::columns_album_changed(const BoolList& list)
{
	m->shown_cols_albums = list;
	_settings->set(Set::Lib_ColsAlbum, list);
}


void  GUI_AbstractLibrary::columns_artist_changed(const BoolList& list)
{
	m->shown_cols_artist = list;
	_settings->set(Set::Lib_ColsArtist, list);
}


void  GUI_AbstractLibrary::columns_title_changed(const BoolList& list)
{
	m->shown_cols_tracks = list;
	_settings->set(Set::Lib_ColsTitle, list);
}


void GUI_AbstractLibrary::sortorder_artist_changed(Library::SortOrder s)
{
	Library::Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_artists = s;

	 _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_album_changed(Library::SortOrder s)
{
    Library::Sortings so = _settings->get(Set::Lib_Sorting);
    so.so_albums = s;

	_settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::sortorder_title_changed(Library::SortOrder s)
{
	Library::Sortings so = _settings->get(Set::Lib_Sorting);
	so.so_tracks = s;

    _settings->set(Set::Lib_Sorting, so);
}


void GUI_AbstractLibrary::delete_current_tracks()
{
    int n_tracks = m->library->get_current_tracks().count();

    Library::TrackDeletionMode answer = show_delete_dialog(n_tracks);
    if(answer != Library::TrackDeletionMode::None) {
        m->library->delete_current_tracks(answer);
    }
}

void GUI_AbstractLibrary::append()
{
	m->library->psl_append_all_tracks();
}

void GUI_AbstractLibrary::append_tracks()
{
    m->library->psl_append_current_tracks();
}

void GUI_AbstractLibrary::play_next()
{
	m->library->psl_play_next_all_tracks();
}

void GUI_AbstractLibrary::play_next_tracks()
{
    m->library->psl_play_next_current_tracks();
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
