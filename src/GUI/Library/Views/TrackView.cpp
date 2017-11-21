#include "TrackView.h"

#include "GUI/Library/Models/TrackModel.h"
#include "GUI/Library/Delegates/RatingDelegate.h"
#include "GUI/Library/Utils/ColumnHeader.h"
#include "GUI/Library/Utils/ColumnIndex.h"

#include "Components/Library/AbstractLibrary.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Library/Sortorder.h"
#include "Utils/Library/Sorting.h"

using namespace Library;

struct TrackView::Private
{
	AbstractLibrary* library = nullptr;
};

TrackView::TrackView(QWidget* parent) :
	Library::TableView(parent)
{
	m = Pimpl::make<Private>();
}

TrackView::~TrackView() {}

void TrackView::init_view(AbstractLibrary* library)
{
	m->library = library;

	TrackModel* track_model = new TrackModel(this, library);
	RatingDelegate* track_delegate = new RatingDelegate(this, (int) ColumnIndex::Track::Rating, true);

	this->setModel(track_model);
	this->setSearchModel(track_model);
	this->setItemDelegate(track_delegate);
	this->set_metadata_interpretation(MD::Interpretation::Tracks);

	connect(this, &ItemView::doubleClicked, this, &TrackView::double_clicked);
	connect(library, &AbstractLibrary::sig_all_tracks_loaded, this, &TrackView::tracks_ready);
}


ColumnHeaderList TrackView::column_headers() const
{
	ColumnHeaderList track_columns;

	ColumnHeader* t_h0 = new ColumnHeader(ColumnHeader::Sharp, true, SortOrder::TrackNumAsc, SortOrder::TrackNumDesc, 25);
	ColumnHeader* t_h1 = new ColumnHeader(ColumnHeader::Title, false, SortOrder::TrackTitleAsc, SortOrder::TrackTitleDesc, 0.4, 200);
	ColumnHeader* t_h2 = new ColumnHeader(ColumnHeader::Artist, true, SortOrder::TrackArtistAsc, SortOrder::TrackArtistDesc, 0.3, 160);
	ColumnHeader* t_h3 = new ColumnHeader(ColumnHeader::Album, true, SortOrder::TrackAlbumAsc, SortOrder::TrackAlbumDesc, 0.3, 160);
	ColumnHeader* t_h4 = new ColumnHeader(ColumnHeader::Year, true, SortOrder::TrackYearAsc, SortOrder::TrackYearDesc, 50);
	ColumnHeader* t_h5 = new ColumnHeader(ColumnHeader::DurationShort, true, SortOrder::TrackLenghtAsc, SortOrder::TrackLengthDesc, 50);
	ColumnHeader* t_h6 = new ColumnHeader(ColumnHeader::Bitrate, true, SortOrder::TrackBitrateAsc, SortOrder::TrackBitrateDesc, 75);
	ColumnHeader* t_h7 = new ColumnHeader(ColumnHeader::Filesize, true, SortOrder::TrackSizeAsc, SortOrder::TrackSizeDesc, 75);
	ColumnHeader* t_h8 = new ColumnHeader(ColumnHeader::Rating, true, SortOrder::TrackRatingAsc, SortOrder::TrackRatingDesc, 80);

	track_columns  << t_h0  << t_h1  << t_h2  << t_h3  <<t_h4  << t_h5  << t_h6 << t_h7 << t_h8;

	return track_columns;
}

BoolList TrackView::shown_columns() const
{
	return _settings->get(Set::Lib_ColsTitle);
}

SortOrder TrackView::sortorder() const
{
	Sortings so = _settings->get(Set::Lib_Sorting);
	return so.so_tracks;
}

void TrackView::sortorder_changed(SortOrder s)
{
	TableView::sortorder_changed(s);
	m->library->change_track_sortorder(s);
}

void TrackView::columns_changed()
{
	TableView::columns_changed();
	_settings->set(Set::Lib_ColsTitle, this->shown_columns());
}

void TrackView::selection_changed(const IndexSet& lst)
{
	TableView::selection_changed(lst);
	m->library->selected_tracks_changed(lst);
}

void TrackView::double_clicked(const QModelIndex& idx)
{
	Q_UNUSED(idx)

	m->library->prepare_current_tracks_for_playlist(false);
}

void TrackView::middle_clicked()
{
	TableView::middle_clicked();
	m->library->prepare_current_tracks_for_playlist(true);
}

void TrackView::play_next_clicked()
{
	TableView::play_next_clicked();
	m->library->play_next_current_tracks();
}

void TrackView::append_clicked()
{
	TableView::append_clicked();
	m->library->append_current_tracks();
}

void TrackView::refresh_clicked()
{
	TableView::refresh_clicked();
	m->library->refresh_tracks();
}

void TrackView::tracks_ready()
{
	const MetaDataList& v_md = m->library->tracks();

	this->fill<MetaDataList, TrackModel>(v_md);
}
