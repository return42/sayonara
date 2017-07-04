/* LibraryViewAlbum.h */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#ifndef LIBRARYVIEWALBUM_H
#define LIBRARYVIEWALBUM_H

#include "Helper/MetaData/Album.h"
#include "GUI/Library/Views/LibraryTableView.h"
#include <QList>
#include <QModelIndex>

class DiscPopupMenu;
class LibraryViewAlbum : public LibraryTableView
{
	Q_OBJECT
	PIMPL(LibraryViewAlbum)

signals:
	void sig_disc_pressed(int);

protected slots:
	void index_clicked(const QModelIndex& idx);

public:
	explicit LibraryViewAlbum(QWidget *parent=nullptr);
	virtual ~LibraryViewAlbum();

private:
	void clear_discnumbers();
	void add_discnumbers(const QList<uint8_t>& dns);
	void calc_discmenu_point(QModelIndex idx);
	void delete_discmenu();
	void init_discmenu(QModelIndex idx);
	void show_discmenu();
	void rc_menu_show(const QPoint& p);

public:
	template <typename T, typename ModelType>
	void fill(const T& input_data)
	{
		clear_discnumbers();

		for(const Album& album : input_data){
			add_discnumbers(album.discnumbers);
		}

		LibraryView::fill<T, ModelType>(input_data);
	}
};


#endif // LIBRARYVIEWALBUM_H
