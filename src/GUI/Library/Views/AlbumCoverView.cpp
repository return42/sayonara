#include "AlbumCoverView.h"
#include <QHeaderView>

AlbumCoverView::AlbumCoverView(QWidget* parent) :
	LibraryView(parent)
{
	set_selection_type( SayonaraSelectionView::SelectionType::Items );
	this->setSelectionBehavior( QAbstractItemView::SelectItems );

	this->setShowGrid(false);
	if(horizontalHeader()){
		horizontalHeader()->hide();
	}

	if(verticalHeader()){
		verticalHeader()->hide();
	}
}

AlbumCoverView::~AlbumCoverView() {}



int AlbumCoverView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row() * model()->columnCount() + idx.column();
}

QModelIndex AlbumCoverView::get_model_index_by_index(int idx) const
{
	int row = idx / model()->columnCount();
	int col = idx / model()->columnCount();

	return model()->index(row, col);
}

MD::Interpretation AlbumCoverView::get_metadata_interpretation() const
{
	return MD::Interpretation::Albums;
}
