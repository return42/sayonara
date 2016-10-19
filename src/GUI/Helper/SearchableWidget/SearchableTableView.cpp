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

#include "GUI/Library/Models/LibraryItemModel.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

SearchableTableView::SearchableTableView(QWidget* parent) :
	QTableView(parent)
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

void SearchableTableView::setAbstractModel(AbstractSearchTableModel* model)
{
	 _abstr_model = model;
	 _abstr_model->set_search_mode(_search_mode);
	 _mini_searcher->set_extra_triggers(_abstr_model->getExtraTriggers());
}


QAbstractItemModel* SearchableTableView::get_model() const
{
	return _abstr_model;
}


QItemSelectionModel* SearchableTableView::get_selection_model() const
{
	return this->selectionModel();
}


void SearchableTableView::set_current_index(int idx)
{
	this->setCurrentIndex(_abstr_model->index(idx, 0));
}


void SearchableTableView::mouseMoveEvent(QMouseEvent *e)
{
	emit sig_mouse_moved();
	QTableView::mouseMoveEvent(e);
}


void SearchableTableView::mousePressEvent(QMouseEvent *e)
{
	emit sig_mouse_pressed();
	QTableView::mousePressEvent(e);
}


void SearchableTableView::mouseReleaseEvent(QMouseEvent *e)
{
	emit sig_mouse_released();
	QTableView::mouseReleaseEvent(e);
}


void SearchableTableView::keyPressEvent(QKeyEvent *e)
{
	bool was_initialized = _mini_searcher->isVisible();
	bool initialized = _mini_searcher->check_and_init(e);

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
	}

	QTableView::keyPressEvent(e);
}


void SearchableTableView::edit_changed(QString str)
{
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QString converted_string = Library::convert_search_string(str, _search_mode);
	QModelIndex idx = _abstr_model->getFirstRowIndexOf(converted_string);

	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::fwd_clicked()
{
	QString str = _mini_searcher->get_current_text();
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QString converted_string = Library::convert_search_string(str, _search_mode);
	QModelIndex idx = _abstr_model->getNextRowIndexOf(converted_string, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::bwd_clicked()
{
	QString str = _mini_searcher->get_current_text();
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QString converted_string = Library::convert_search_string(str, _search_mode);
	QModelIndex idx = _abstr_model->getPrevRowIndexOf(converted_string, _cur_row -1);

	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->selectRow(_cur_row);
}


void SearchableTableView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QTableView::selectionChanged(selected, deselected);

	emit sig_selection_changed(selected.indexes());
}

void SearchableTableView::search_mode_changed()
{
	_search_mode = _settings->get(Set::Lib_SearchMode);
	if(_abstr_model){
		_abstr_model->set_search_mode(_search_mode);
	}
}

Library::SearchModeMask SearchableTableView::search_mode() const
{
	return _search_mode;
}
