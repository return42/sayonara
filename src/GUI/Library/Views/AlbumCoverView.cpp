#include "AlbumCoverView.h"
#include <QHeaderView>
#include "GUI/Library/Delegates/AlbumCoverDelegate.h"

AlbumCoverView::AlbumCoverView(QWidget* parent) :
	LibraryView(parent)
{

}

AlbumCoverView::~AlbumCoverView() {}


int AlbumCoverView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.column();
}
