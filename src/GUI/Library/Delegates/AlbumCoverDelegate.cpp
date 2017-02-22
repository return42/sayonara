#include "AlbumCoverDelegate.h"
#include <QPainter>
AlbumCoverDelegate::AlbumCoverDelegate(QObject* parent) :
	QItemDelegate(parent)
{}

AlbumCoverDelegate::~AlbumCoverDelegate() {}


void AlbumCoverDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	painter->save();
	int offset = 20;
	painter->translate(0, offset);
	QStyleOptionViewItem option_new = option;
	QRect rect = option.rect;
	rect.setHeight(rect.height() - offset);
	option_new.rect = rect;

	QItemDelegate::paint(painter, option_new, index);

	painter->restore();
}
