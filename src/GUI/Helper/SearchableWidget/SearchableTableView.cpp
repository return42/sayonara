/* SearchableTableView.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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

#include "SearchableTableView.h"
#include "AbstractSearchModel.h"
#include "MiniSearcher.h"
#include "Helper/Set.h"
#include "Helper/Settings/Settings.h"

SearchableTableView::SearchableTableView(QWidget* parent) :
	QTableView(parent),
	SayonaraSelectionView()
{
	_settings = Settings::getInstance();
	_mini_searcher = new MiniSearcher(this, MiniSearcherButtons::BothButtons);
	_abstr_model = nullptr;
	_cur_row = -1;

	connect(_mini_searcher, &MiniSearcher::sig_text_changed, this, &SearchableTableView::edit_changed);
	connect(_mini_searcher, &MiniSearcher::sig_find_next_row, this, &SearchableTableView::fwd_clicked);
	connect(_mini_searcher, &MiniSearcher::sig_find_prev_row, this, &SearchableTableView::bwd_clicked);

	REGISTER_LISTENER(Set::Lib_SearchMode, search_mode_changed);
}

SearchableTableView::~SearchableTableView() {}

void SearchableTableView::setSearchModel(SearchModelInterface* model)
{
	 _abstr_model = model;

	 if(_abstr_model){
		 _abstr_model->set_search_mode(_settings->get(Set::Lib_SearchMode));
	 }

	 _mini_searcher->set_extra_triggers(_abstr_model->getExtraTriggers());
}

int SearchableTableView::get_row_count() const
{
	return model()->rowCount();
}

int SearchableTableView::get_column_count() const
{
	return model()->columnCount();
}

QModelIndex SearchableTableView::get_index(int row, int col) const
{
	return model()->index(row, col);
}

QItemSelectionModel* SearchableTableView::get_selection_model() const
{
	return this->selectionModel();
}

void SearchableTableView::set_current_index(int idx)
{
	this->setCurrentIndex(get_index(idx, 0));
}

void SearchableTableView::keyPressEvent(QKeyEvent *e) 
{
	bool was_initialized = _mini_searcher->isVisible();
	bool initialized = _mini_searcher->check_and_init(e);

	if(e->key() == Qt::Key_Tab && !was_initialized) {
		return;
	}

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
		return;
	}

	QTableView::keyPressEvent(e);
	e->setAccepted(true);
}

QModelIndex SearchableTableView::get_match_index(const QString& str, SearchDirection direction) const
{
	QModelIndex idx;
	if(str.isEmpty()) {
		return idx;
	}

	if(!_abstr_model) {
		return idx;
	}

	QMap<QChar, QString> extra_triggers = _abstr_model->getExtraTriggers();
	Library::SearchModeMask search_mode = _settings->get(Set::Lib_SearchMode);

	QString converted_string = Library::convert_search_string(str, search_mode, extra_triggers.keys());

	switch(direction)
	{
		case SearchDirection::First:
			idx = _abstr_model->getFirstRowIndexOf(converted_string);
			break;
		case SearchDirection::Next:
			idx = _abstr_model->getNextRowIndexOf(converted_string, _cur_row + 1);
			break;
		case SearchDirection::Prev:
			idx = _abstr_model->getPrevRowIndexOf(converted_string, _cur_row - 1);
			break;
	}

	return idx;
}


void SearchableTableView::select_match(const QString &str, SearchDirection direction)
{
	QModelIndex idx = get_match_index(str, direction);
	if(!idx.isValid()){
		_cur_row = -1;
		return;
	}

	_cur_row = idx.row();

	this->setCurrentIndex(idx);

	SP::Set<int> indexes;
	indexes.insert(_cur_row);

	this->select_rows(indexes);
	this->scrollTo(idx);
}

void SearchableTableView::edit_changed(const QString& str)
{
	select_match(str, SearchDirection::First);
}

void SearchableTableView::fwd_clicked() 
{
	QString str = _mini_searcher->get_current_text();
	select_match(str, SearchDirection::Next);
}

void SearchableTableView::bwd_clicked() 
{
	QString str = _mini_searcher->get_current_text();
	select_match(str, SearchDirection::Prev);
}

void SearchableTableView::search_mode_changed()
{
	Library::SearchModeMask search_mode = _settings->get(Set::Lib_SearchMode);

	if(_abstr_model){
		_abstr_model->set_search_mode(search_mode);
	}
}
