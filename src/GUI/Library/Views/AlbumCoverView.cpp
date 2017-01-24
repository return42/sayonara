#include "AlbumCoverView.h"

#include <QHeaderView>
#include <QWheelEvent>

struct AlbumCoverView::Private
{
	int zoom;

	Private()
	{
		zoom = 100;
	}
};

AlbumCoverView::AlbumCoverView(QWidget* parent) :
	LibraryView(parent)
{
	_m = Pimpl::make<Private>();

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
	int col = idx % model()->columnCount();

	return model()->index(row, col);
}

MD::Interpretation AlbumCoverView::get_metadata_interpretation() const
{
	return MD::Interpretation::Albums;
}

void AlbumCoverView::wheelEvent(QWheelEvent* e)
{
	if(e->modifiers() & Qt::ControlModifier){

		if(e->delta() > 0){
			_m->zoom = std::min(_m->zoom + 10, 200);
		}
		else{
			_m->zoom = std::max(_m->zoom - 10, 50);
		}

		emit sig_zoom_changed(_m->zoom);
	}

	LibraryView::wheelEvent(e);

}
