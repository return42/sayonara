/* LibraryItemDelegateArtists.h */

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
 * LibraryItemDelegateArtists.h
 *
 *  Created on: Jun 3, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMDELEGATEARTISTS_H_
#define LIBRARYITEMDELEGATEARTISTS_H_

#include <QObject>
#include <QTableView>
#include <QStyledItemDelegate>


class LibraryItemDelegateArtists :
		public QStyledItemDelegate
{

public:
	LibraryItemDelegateArtists(QObject* parent=nullptr);
	virtual ~LibraryItemDelegateArtists();


private:

		QPixmap			_icon_single_album;
		QPixmap			_icon_multi_album;
};

#endif /* LIBRARYITEMDELEGATEARTISTS_H_ */
