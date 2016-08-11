/* AlternateCoverItemDelegate.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * AlternateCoverItemDelegate.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: Lucio Carreras
 */

#include "AlternativeCoverItemDelegate.h"
#include "Components/CoverLookup/CoverLookup.h"
#include "Components/CoverLookup/CoverLocation.h"

#include "GUI/Helper/GUI_Helper.h"
#include "Helper/Logger/Logger.h"

#include <QPainter>



AlternateCoverItemDelegate::AlternateCoverItemDelegate(QObject* parent) : QItemDelegate(parent) {

	QPixmap pm = GUI::get_pixmap("logo.png");

	label = new QLabel();
	label->setScaledContents(true);
	label->setStyleSheet("background: transparent;");

	label->setPixmap(pm);
}

AlternateCoverItemDelegate::~AlternateCoverItemDelegate() {

}


void AlternateCoverItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
	QItemDelegate::paint(painter, option, index);

    if(!index.isValid()) return;

	QRect rect(option.rect);

	painter->save();
	painter->translate(2, 0);

	QVariant var = index.model()->data(index);
	CoverLocation cl = CoverLocation::getInvalidLocation();

	if( var.canConvert<CoverLocation>()){
		cl = var.value<CoverLocation>();
	}

	label->setEnabled(cl.valid);
	label->setMinimumHeight(100);
	label->setMinimumWidth(100);
    label->resize(100, 100);

	label->setContentsMargins(10, 10, 10, 10);

	QPixmap pixmap( cl.cover_path );

	if(!pixmap.isNull()) {

		label->setPixmap(pixmap);
	}

	label->render(painter, rect.topLeft() );

    painter->restore();

}


QSize AlternateCoverItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	Q_UNUSED(option)
	Q_UNUSED(index)
	return QSize(100, 100);
}

