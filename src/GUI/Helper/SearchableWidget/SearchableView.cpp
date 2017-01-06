/* SearchableView.cpp */

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

#include "SearchableView.h"
#include "AbstractSearchModel.h"
#include "MiniSearcher.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Set.h"

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QMap>
#include <QString>

struct SearchViewInterface::Private
{
	SearchModelInterface*		search_model=nullptr;
	QAbstractItemView*			view=nullptr;
	int							cur_row;
};


SearchViewInterface::SearchViewInterface(QAbstractItemView* view) :
	SayonaraSelectionView()
{
	_settings = Settings::getInstance();

	_m = Pimpl::make<SearchViewInterface::Private>();

	_m->view = view;
	_m->search_model = nullptr;
	_m->cur_row = -1;
}

SearchViewInterface::~SearchViewInterface() {}

void SearchViewInterface::setSearchModel(SearchModelInterface* model)
{
	 _m->search_model = model;

	 if(_m->search_model){
		 Library::SearchModeMask search_mode = _settings->get(Set::Lib_SearchMode);
		 _m->search_model->set_search_mode(search_mode);
	 }

	 mini_searcher()->set_extra_triggers(_m->search_model->getExtraTriggers());
}


int SearchViewInterface::get_row_count(const QModelIndex& parent) const
{
	return _m->view->model()->rowCount(parent);
}


int SearchViewInterface::get_column_count(const QModelIndex& parent) const
{
	return _m->view->model()->columnCount(parent);
}


QModelIndex SearchViewInterface::get_index(int row, int col, const QModelIndex& parent) const
{
	return _m->view->model()->index(row, col, parent);
}


QModelIndex SearchViewInterface::get_match_index(const QString& str, SearchDirection direction) const
{
	QModelIndex idx;
	if(str.isEmpty()) {
		return idx;
	}

	if(!_m->search_model) {
		return idx;
	}

	Library::SearchModeMask search_mode = _settings->get(Set::Lib_SearchMode);
	QMap<QChar, QString> extra_triggers = _m->search_model->getExtraTriggers();

	QString converted_string = Library::convert_search_string(str, search_mode, extra_triggers.keys());

	switch(direction)
	{
		case SearchDirection::First:
			idx = _m->search_model->getFirstRowIndexOf(converted_string);
			break;
		case SearchDirection::Next:
			idx = _m->search_model->getNextRowIndexOf(converted_string, _m->cur_row + 1);
			break;
		case SearchDirection::Prev:
			idx = _m->search_model->getPrevRowIndexOf(converted_string, _m->cur_row - 1);
			break;
	}

	return idx;
}


void SearchViewInterface::select_match(const QString &str, SearchDirection direction)
{
	QModelIndex idx = get_match_index(str, direction);
	if(!idx.isValid()){
		_m->cur_row = -1;
		return;
	}

	_m->cur_row = idx.row();

	SP::Set<int> indexes;
	indexes.insert(_m->cur_row);

	this->select_rows(indexes);
	this->set_current_index(_m->cur_row);

	_m->view->scrollTo(idx);
}


QItemSelectionModel* SearchViewInterface::get_selection_model() const
{
	return _m->view->selectionModel();
}


void SearchViewInterface::set_current_index(int idx)
{
	_m->view->setCurrentIndex(get_index(idx, 0));
}


void SearchViewInterface::handleKeyPress(QKeyEvent* e)
{
	if(!_m->search_model)
	{
		return;
	}

	Library::SearchModeMask search_mode = _settings->get(Set::Lib_SearchMode);
	_m->search_model->set_search_mode(search_mode);

	MiniSearcher* searcher = mini_searcher();
	bool was_initialized = searcher->isVisible();
	bool initialized = searcher->check_and_init(e);

	if(e->key() == Qt::Key_Tab && !was_initialized) {
		return;
	}

	if(initialized || was_initialized) {
		searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}

	e->setAccepted(true);
}
