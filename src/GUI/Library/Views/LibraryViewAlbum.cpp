/* LibraryViewAlbum.cpp */

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

#include "LibraryViewAlbum.h"
#include "GUI/Library/Helper/DiscPopupMenu.h"
#include "GUI/Library/Helper/ColumnIndex.h"

#include <QHeaderView>

struct LibraryViewAlbum::Private
{
	QList< QList<uint8_t> >	discnumbers;
	DiscPopupMenu*			discmenu=nullptr;
	QPoint					discmenu_point;
};

LibraryViewAlbum::LibraryViewAlbum(QWidget *parent) :
	LibraryTableView(parent)
{
	_m = Pimpl::make<Private>();
	connect(this, &QTableView::clicked, this, &LibraryViewAlbum::index_clicked);
}

LibraryViewAlbum::~LibraryViewAlbum() {}

void LibraryViewAlbum::rc_menu_show(const QPoint & p)
{
	delete_discmenu();

	LibraryTableView::rc_menu_show(p);
}


void LibraryViewAlbum::index_clicked(const QModelIndex &idx)
{
	if(idx.column() != static_cast<int>(ColumnIndex::Album::MultiDisc))
	{
		return;
	}

	QModelIndexList selections = this->selectionModel()->selectedRows();
	if(selections.size() == 1){
		init_discmenu(idx);
		show_discmenu();
	}
}


/* where to show the popup */
void LibraryViewAlbum::calc_discmenu_point(QModelIndex idx)
{
	_m->discmenu_point = QCursor::pos();

	QRect box = this->geometry();
	box.moveTopLeft(this->parentWidget()->mapToGlobal(box.topLeft()));

	if(!box.contains(_m->discmenu_point)){
		_m->discmenu_point.setX(box.x() + (box.width() * 2) / 3);
		_m->discmenu_point.setY(box.y());

		QPoint dmp_tmp = parentWidget()->pos();
		dmp_tmp.setY(dmp_tmp.y() - this->verticalHeader()->sizeHint().height());

		while(idx.row() != indexAt(dmp_tmp).row()){
			  dmp_tmp.setY(dmp_tmp.y() + 10);
			  _m->discmenu_point.setY(_m->discmenu_point.y() + 10);
		}
	}
}

void LibraryViewAlbum::init_discmenu(QModelIndex idx)
{
	int row = idx.row();
	QList<uint8_t> discnumbers;
	delete_discmenu();

	if( !idx.isValid() ||
		(row > _m->discnumbers.size()) ||
		(row < 0) )
	{
		return;
	}

	discnumbers = _m->discnumbers[row];
	if(discnumbers.size() < 2) {
		return;
	}

	calc_discmenu_point(idx);

	_m->discmenu = new DiscPopupMenu(this, discnumbers);

	connect(_m->discmenu, &DiscPopupMenu::sig_disc_pressed, this, &LibraryViewAlbum::sig_disc_pressed);
}


void LibraryViewAlbum::delete_discmenu()
{
	if(!_m->discmenu) {
		return;
	}

	_m->discmenu->hide();
	_m->discmenu->close();

	disconnect(_m->discmenu, &DiscPopupMenu::sig_disc_pressed, this, &LibraryViewAlbum::sig_disc_pressed);

	_m->discmenu->deleteLater();
	_m->discmenu = nullptr;
}


void LibraryViewAlbum::show_discmenu()
{
	if(!_m->discmenu) return;

	_m->discmenu->popup(_m->discmenu_point);
}


void LibraryViewAlbum::clear_discnumbers()
{
	_m->discnumbers.clear();
}

void LibraryViewAlbum::add_discnumbers(const QList<uint8_t>& dns)
{
	_m->discnumbers << dns;
}
