/* ListDelegate.cpp */

#include "ListDelegate.h"
#include <QPalette>
#include <QColor>
#include <QStyle>
#include <QPainter>

ListDelegate::ListDelegate(QListView* parent) :
	QItemDelegate(parent),
	SayonaraClass()
{
	_parent = parent;
}

ListDelegate::~ListDelegate(){

}


QSize ListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	return QSize(0, 20);
}


