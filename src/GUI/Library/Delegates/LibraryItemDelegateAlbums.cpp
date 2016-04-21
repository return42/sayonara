/* LibraryItemDelegateAlbums.cpp */

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
 * LibraryItemDelegateAlbums.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: luke
 */

#include "LibraryItemDelegateAlbums.h"
#include "GUI/Library/Views/LibraryView.h"
#include "GUI/Library/Models/LibraryItemModelAlbums.h"
#include "GUI/Helper/RatingLabel/RatingLabel.h"
#include "Helper/Helper.h"

#include <QLabel>
#include <QItemDelegate>
#include <QPainter>
#include <QTableView>
#include <QStyleOptionViewItem>


LibraryItemDelegateAlbums::LibraryItemDelegateAlbums(LibraryView* parent, bool enabled) :
	LibraryRatingDelegate(parent, enabled)
{
	_icon_single_album = Helper::get_pixmap("play", QSize(16, 16), false);
	_icon_multi_album = Helper::get_pixmap("sampler", QSize(16, 16), false);
}


LibraryItemDelegateAlbums::~LibraryItemDelegateAlbums() {

}


void LibraryItemDelegateAlbums::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(!index.isValid()) return;

	QRect rect(option.rect);

	painter->save();

	const LibraryItemModelAlbums* model = reinterpret_cast<const LibraryItemModelAlbums*>(index.model());

	int col = index.column();
	int idx_col = model->calc_shown_col(col);

	if(idx_col == COL_ALBUM_SAMPLER) {
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


	else if(idx_col == COL_ALBUM_NAME) {

		rect.translate(2, 0);

		QString name = index.data().toString();
		painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, name);
	}


	else if(idx_col == COL_ALBUM_YEAR) {

		rect.translate(-2, 0);
		int year = index.data().toInt();

		QString year_str = QString::number(year);
		if(year == 0) year_str = tr("None");
		painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, year_str);

	}

	else if(idx_col == COL_ALBUM_N_SONGS) {

		rect.translate(-2, 0);
		QString n_songs = index.data().toString() + " " + tr("tracks");
		painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, n_songs);
	}

	else if(idx_col == COL_ALBUM_DURATION) {

		rect.translate(-2, 0);
		QString duration = index.data().toString();
		painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, duration);
	}

	else if(idx_col == COL_ALBUM_RATING) {

		quint8 r = index.data().toInt();
		Rating rating(r);
		rating.paint(painter, rect, option.palette, false);
	}

	painter->restore();
}
