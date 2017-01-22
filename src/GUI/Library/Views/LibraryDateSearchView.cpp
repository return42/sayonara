/* LibraryDateSearchView.cpp */

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

#include "LibraryDateSearchView.h"
#include "GUI/Library/Models/DateSearchModel.h"
#include "GUI/Library/GUI_DateSearchConfig.h"
#include "GUI/Helper/ContextMenu/ContextMenu.h"
#include "GUI/Helper/Delegates/ListDelegate.h"
#include "Helper/Language.h"
#include "Helper/Library/DateFilter.h"

#include <QContextMenuEvent>

struct LibraryDateSearchView::Private
{
	ContextMenu*			rc_menu=nullptr;
	GUI_DateSearchConfig*	dsc=nullptr;
	DateSearchModel*		model=nullptr;

	void check_dsc(QWidget* parent){
		if(!dsc){
			dsc = new GUI_DateSearchConfig(parent);
		}
	}
};

LibraryDateSearchView::LibraryDateSearchView(QWidget* parent) :
	SearchableListView(parent)
{
	_m = Pimpl::make<LibraryDateSearchView::Private>();
	_m->model = new DateSearchModel(this);

	this->setModel(_m->model);
	this->setSearchModel(_m->model);
	this->setItemDelegate(new ListDelegate(this));
}

LibraryDateSearchView::~LibraryDateSearchView() {}


Library::DateFilter LibraryDateSearchView::get_filter(int row) const
{
	return _m->model->get_filter(row);
}


void LibraryDateSearchView::contextMenuEvent(QContextMenuEvent* e)
{
	if(!_m->rc_menu)
	{
		_m->rc_menu = new ContextMenu(this);
		_m->rc_menu->show_actions(
					ContextMenu::EntryNew |
					ContextMenu::EntryEdit |
					ContextMenu::EntryDelete
		);

		connect(_m->rc_menu, &ContextMenu::sig_new, this, &LibraryDateSearchView::new_clicked);
		connect(_m->rc_menu, &ContextMenu::sig_edit, this, &LibraryDateSearchView::edit_clicked);
		connect(_m->rc_menu, &ContextMenu::sig_delete, this, &LibraryDateSearchView::delete_clicked);
	}

	QPoint pos = e->globalPos();
	_m->rc_menu->exec(pos);

	SearchableListView::contextMenuEvent(e);
}


void LibraryDateSearchView::new_clicked()
{
	_m->check_dsc(this);
	_m->dsc->set_filter(Library::DateFilter());
	_m->dsc->exec();

	Library::DateFilter edited_filter = _m->dsc->get_edited_filter();
	if(!edited_filter.valid()){
		return;
	}

	GUI_DateSearchConfig::Result result = _m->dsc->get_result();

	if(result !=  GUI_DateSearchConfig::Result::Cancelled){
		_m->model->add_data(edited_filter);
	}

	return;
}


void LibraryDateSearchView::edit_clicked()
{
	QModelIndex cur_idx = this->currentIndex();
	Library::DateFilter filter = _m->model->get_filter(cur_idx.row());

	_m->check_dsc(this);
	_m->dsc->set_filter(filter);
	_m->dsc->exec();

	Library::DateFilter edited_filter = _m->dsc->get_edited_filter();
	GUI_DateSearchConfig::Result result = _m->dsc->get_result();

	if(!edited_filter.valid()){
		return;
	}

	if(result ==  GUI_DateSearchConfig::Result::New){
		_m->model->add_data(edited_filter);
	}

	else if(result ==  GUI_DateSearchConfig::Result::Replace){
		_m->model->set_data(edited_filter, this->currentIndex().row());
	}
}


void LibraryDateSearchView::delete_clicked()
{
	QModelIndex cur_idx = this->currentIndex();
	_m->model->remove(cur_idx.row());
}


int LibraryDateSearchView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex LibraryDateSearchView::get_model_index_by_index(int idx) const
{
	return _m->model->index(idx, 0);
}
