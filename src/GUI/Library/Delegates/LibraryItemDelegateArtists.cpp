/* LibraryItemDelegateArtists.cpp */

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
 * LibraryItemDelegateArtists.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "LibraryItemDelegateArtists.h"
#include "GUI/Helper/GUI_Helper.h"

#include <QLabel>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>


LibraryItemDelegateArtists::LibraryItemDelegateArtists(QTableView* parent) :
	QItemDelegate(parent)
{

	_parent = parent;

	_icon_single_album = GUI::get_pixmap("play", QSize(16, 16), false);
	_icon_multi_album = GUI::get_pixmap("sampler", QSize(16, 16), false);

}

LibraryItemDelegateArtists::~LibraryItemDelegateArtists() {

}


void LibraryItemDelegateArtists::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{

    if(!index.isValid()) return;

    QRect rect(option.rect);
	painter->save();

	const LibraryItemModelArtists* model = reinterpret_cast<const LibraryItemModelArtists*>(index.model());

    int col = index.column();
	//int idx_col = model->calc_shown_col(col);
	int idx_col = col;

    if(idx_col == COL_ARTIST_N_ALBUMS) {
        int col_width = _parent->columnWidth(0)-4;
        int row_height = _parent->rowHeight(0)-4;
        rect.translate(2, 2);

		int num_albums = index.data().toInt();

		if(num_albums <= 1){
			painter->drawPixmap(rect.x(), rect.y(), col_width, row_height, _icon_single_album);
		}

		else{
            painter->drawPixmap(rect.x(), rect.y(), col_width, row_height, _icon_multi_album);
		}

    }

    else if(idx_col == COL_ARTIST_NAME) {

        rect.translate(2, 0);
		QString name = index.data().toString();

        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, name);

    }

    else if(idx_col == COL_ARTIST_TRACKS) {

        rect.translate(-2, 0);
		int n_tracks = index.data().toInt();

		painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, QString::number(n_tracks) + " " + tr("tracks"));

    }

    painter->restore();
}
