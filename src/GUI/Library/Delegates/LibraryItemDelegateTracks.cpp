/* LibraryItemDelegateTracks.cpp */

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
 * LibraryItemDelegateTracks.cpp
 *
 *  Created on: Oct 28, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryItemDelegateTracks.h"
#include "GUI/Library/Models/LibraryItemModelTracks.h"
#include "GUI/Helper/RatingLabel/RatingLabel.h"

#include "Helper/FileHelper.h"

#include <QLabel>
#include <QTableView>
#include <QItemDelegate>
#include <QPainter>

LibraryItemDelegateTracks::LibraryItemDelegateTracks(LibraryView* parent, bool enabled) :
	LibraryRatingDelegate(parent, enabled)
{

}

LibraryItemDelegateTracks::~LibraryItemDelegateTracks() {

}

void LibraryItemDelegateTracks::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if(!index.isValid()) return;

	QRect 	rect(option.rect);
	painter->save();

    int col = index.column();

    QString	text = index.data().toString();

	if(col == COL_FILESIZE) {
		text = Helper::File::calc_filesize_str(text.toInt());
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

	else if(col == COL_BITRATE) {
        text = QString::number(text.toInt() / 1000) + " kbit/s";
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
	else if(col == COL_YEAR) {
            if(text == "0") text = "";
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
	else if(col == COL_TRACK_NUM) {
            painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }
	else if(col == COL_LENGTH) {
        rect.translate(-2, 0);
        painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

	else if(col == COL_TRACK_RATING) {
		quint8 r = index.data().toInt();
        Rating rating(r);
		rating.paint(painter, rect, option.palette, false);
    }

	else if(col == COL_TITLE){
	    rect.translate(3, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
	}

    else{
        rect.translate(2, 0);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }

    painter->restore();
}
