/* LibraryView.cpp */

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
 * LibraryView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#include "LibraryView.h"

#include "GUI/Library/Models/LibraryItemModel.h"
#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/SearchableWidget/MiniSearcher.h"


LibraryView::LibraryView(QWidget* parent) : SearchableTableView(parent) {

	_drag = nullptr;
	_rc_header_menu = nullptr;
	_cur_filling = false;
	_model = nullptr;

	rc_menu_init();

	connect(this->horizontalHeader(), &QHeaderView::sectionClicked, this, &LibraryView::sort_by_column);

	setAcceptDrops(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	clearSelection();
}


LibraryView::~LibraryView() {
	delete _rc_menu;
}


void LibraryView::setModel(LibraryItemModel * model) {

	SearchableTableView::setModel(model);
	_model = model;
}


void LibraryView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected ) {

	if(_cur_filling) {
		return;
	}

	SearchableTableView::selectionChanged(selected, deselected);
	IdxList idx_list_int = get_selections();

	emit sig_sel_changed(idx_list_int);
}


void LibraryView::save_selections()
{
	IdxList idx_list_int = get_selections();
	_model->add_selections(idx_list_int);
}
// selections end



void LibraryView::do_drag(){

	CustomMimeData* mimedata = _model->get_mimedata();

	if(_drag){
		delete _drag;
	}

	_drag = new QDrag(this);

	connect(_drag, &QDrag::destroyed, this, &LibraryView::drag_deleted);

	_drag->setMimeData(mimedata);
	_drag->exec();
}


void LibraryView::drag_deleted() {
	_drag = nullptr;
}


// Right click stuff
void LibraryView::rc_menu_init() {
	_rc_menu = new LibraryContextMenu(this);

	set_rc_menu(LibraryContextMenu::EntryPlayNext |
				LibraryContextMenu::EntryInfo |
				LibraryContextMenu::EntryDelete |
				LibraryContextMenu::EntryEdit |
				LibraryContextMenu::EntryAppend);
}

void LibraryView::set_rc_menu(int entries){
	if(!_rc_menu) return;
	_rc_menu->show_actions(entries);
}

MetaDataList LibraryView::get_selected_metadata() const
{
	CustomMimeData* mimedata;
	MetaDataList v_md;

	mimedata = _model->get_mimedata();
	if(mimedata){
		mimedata->getMetaData(v_md);
		delete mimedata;
	}

	return v_md;
}


void LibraryView::rc_menu_show(const QPoint& p) {

	connect(_rc_menu, &LibraryContextMenu::sig_edit_clicked, this, &LibraryView::sig_edit_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_info_clicked, this, &LibraryView::sig_info_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_delete_clicked, this, &LibraryView::sig_delete_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_play_next_clicked, this, &LibraryView::sig_play_next_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_append_clicked, this, &LibraryView::sig_append_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_refresh_clicked, this, &LibraryView::sig_refresh_clicked);

	_rc_menu->exec(p);

	disconnect(_rc_menu, &LibraryContextMenu::sig_edit_clicked, this, &LibraryView::sig_edit_clicked);
	disconnect(_rc_menu, &LibraryContextMenu::sig_info_clicked, this, &LibraryView::sig_info_clicked);
	disconnect(_rc_menu, &LibraryContextMenu::sig_delete_clicked, this, &LibraryView::sig_delete_clicked);
	disconnect(_rc_menu, &LibraryContextMenu::sig_play_next_clicked, this, &LibraryView::sig_play_next_clicked);
	disconnect(_rc_menu, &LibraryContextMenu::sig_append_clicked, this, &LibraryView::sig_append_clicked);
	disconnect(_rc_menu, &LibraryContextMenu::sig_refresh_clicked, this, &LibraryView::sig_refresh_clicked);
}
// right click stuff end
