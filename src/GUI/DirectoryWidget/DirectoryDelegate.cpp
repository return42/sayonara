/* DirectoryDelegate.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#include "DirectoryDelegate.h"
#include "Helper/Logger/Logger.h"
#include <QPainter>

DirectoryDelegate::DirectoryDelegate(QObject* parent) :
	QItemDelegate(parent)
{}

DirectoryDelegate::~DirectoryDelegate()
{}

void DirectoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QIcon::State icon_state;

	if(option.state & QStyle::State_Open){
		icon_state = QIcon::On;
	}
	else{
		icon_state = QIcon::Off;
	}
	QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
	QPixmap pm = icon.pixmap(16, 16, QIcon::Normal, icon_state);


	QItemDelegate::drawBackground(painter, option, index);
	QItemDelegate::drawDecoration(painter, option, QRect(option.rect.left() + 2, option.rect.top(), 16, 16), pm);


	if(!index.isValid()) return;

	QRect rect(option.rect);
	painter->save();
	QString text = index.data().toString();
	rect.translate(24, 0);


	painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
	painter->restore();

}

QSize DirectoryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)

	return QSize(0, 20);
}

