#include "AlbumCoverDelegate.h"

AlbumCoverDelegate::AlbumCoverDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{

}

AlbumCoverDelegate::~AlbumCoverDelegate()
{

}

QSize AlbumCoverDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
	int row = index.row();
	if(row == 0){
		return QSize(100, 100);
	}

	return QStyledItemDelegate::sizeHint(option, index);
}

