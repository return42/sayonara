#include "ListDelegate.h"
#include <QPalette>
#include <QColor>
#include <QStyle>
#include <QPainter>

ListDelegate::ListDelegate(QListView* parent) :
	QStyledItemDelegate(parent),
	SayonaraClass()
{
	_parent = parent;
}

ListDelegate::~ListDelegate(){

}


QSize ListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QSize(0, 20);
}


void ListDelegate::paint( QPainter *painter, 
		const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{


    if(!index.isValid()) {
		return;
	}

	if(!_parent){
		return;
	}

	QRect rect(option.rect);
	QString text = index.data().toString();

    painter->save();
	if(option.state & QStyle::State_Selected){

        QPalette palette = _parent->palette();
        QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);

        painter->fillRect(rect, col_highlight);
    } 
	painter->translate(3, 0);
	painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
	painter->restore();

}
