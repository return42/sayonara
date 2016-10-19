/* SearchableListView.cpp */

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



#include "SearchableListView.h"
#include "AbstractSearchModel.h"
#include "MiniSearcher.h"
#include "Helper/Set.h"
#include "Helper/Settings/Settings.h"

SearchableListView::SearchableListView(QWidget* parent) :
	QListView(parent),
	SayonaraSelectionView(),
	SayonaraClass()
{
	_mini_searcher = new MiniSearcher(this, MiniSearcherButtons::BothButtons);
	_abstr_model = nullptr;
	_cur_row = -1;

	connect(_mini_searcher, &MiniSearcher::sig_text_changed, this, &SearchableListView::edit_changed);
	connect(_mini_searcher, &MiniSearcher::sig_find_next_row, this, &SearchableListView::fwd_clicked);
	connect(_mini_searcher, &MiniSearcher::sig_find_prev_row, this, &SearchableListView::bwd_clicked);

	REGISTER_LISTENER(Set::Lib_SearchMode, search_mode_changed);
}

SearchableListView::~SearchableListView() {}

void SearchableListView::setAbstractModel(AbstractSearchListModel* model) 
{
	 _abstr_model = model;
	 _abstr_model->set_search_mode(_search_mode);
	 _mini_searcher->set_extra_triggers(_abstr_model->getExtraTriggers());
}

QAbstractItemModel* SearchableListView::get_model() const
{
	return _abstr_model;
}

QItemSelectionModel* SearchableListView::get_selection_model() const
{
	return this->selectionModel();
}

void SearchableListView::set_current_index(int idx)
{
	this->setCurrentIndex(_abstr_model->index(idx, 0));
}

void SearchableListView::mouseMoveEvent(QMouseEvent *e) 
{
	emit sig_mouse_moved();
	QListView::mouseMoveEvent(e);
}

void SearchableListView::mousePressEvent(QMouseEvent *e) 
{
	emit sig_mouse_pressed();
	QListView::mousePressEvent(e);
}


void SearchableListView::mouseReleaseEvent(QMouseEvent *e) 
{
	emit sig_mouse_released();
	QListView::mouseReleaseEvent(e);
}

void SearchableListView::keyPressEvent(QKeyEvent *e) 
{
	bool was_initialized = _mini_searcher->isVisible();
	bool initialized = _mini_searcher->check_and_init(e);

	if(initialized || was_initialized) {
		_mini_searcher->keyPressEvent(e);
		e->setAccepted(false);
	}

	QListView::keyPressEvent(e);
}


void SearchableListView::edit_changed(const QString& str) 
{
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QMap<QChar, QString> edit_triggers = _abstr_model->getExtraTriggers();
	QList<QChar> ignored_chars = edit_triggers.keys();

	QString converted_string = Library::convert_search_string(str, _search_mode, ignored_chars);
	QModelIndex idx = _abstr_model->getFirstRowIndexOf(converted_string);

	if(!idx.isValid()) return;

	_cur_row = idx.row();

	this->scrollTo(idx);
	this->setCurrentIndex(idx);

	SP::Set<int> indexes;
	indexes.insert(_cur_row);
	this->select_rows(indexes);
}

void SearchableListView::fwd_clicked() 
{
	QString str = _mini_searcher->get_current_text();
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QMap<QChar, QString> edit_triggers = _abstr_model->getExtraTriggers();
	QList<QChar> ignored_chars = edit_triggers.keys();

	QString converted_string = Library::convert_search_string(str, _search_mode, ignored_chars);
	QModelIndex idx = _abstr_model->getNextRowIndexOf(converted_string, _cur_row + 1);
	if(!idx.isValid()) return;

	_cur_row = idx.row();

	SP::Set<int> indexes;
	indexes.insert(idx.row());

	this->scrollTo(idx);
	this->select_rows(indexes);
}

void SearchableListView::bwd_clicked() 
{
	QString str = _mini_searcher->get_current_text();
	if(str.isEmpty()) return;
	if(!_abstr_model) return;

	QMap<QChar, QString> edit_triggers = _abstr_model->getExtraTriggers();
	QList<QChar> ignored_chars = edit_triggers.keys();

	QString converted_string = Library::convert_search_string(str, _search_mode, ignored_chars);
	QModelIndex idx = _abstr_model->getPrevRowIndexOf(converted_string, _cur_row -1);

	if(!idx.isValid()) return;

	_cur_row = idx.row();

	SP::Set<int> indexes;
	indexes.insert(idx.row());

	this->scrollTo(idx);
	this->select_rows(indexes);
}

void SearchableListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	QListView::selectionChanged(selected, deselected);

	emit sig_selection_changed(selected.indexes());
}

void SearchableListView::search_mode_changed()
{
	_search_mode = _settings->get(Set::Lib_SearchMode);

	if(_abstr_model){
		_abstr_model->set_search_mode(_search_mode);
	}
}

Library::SearchModeMask SearchableListView::search_mode() const
{
	return _search_mode;
}
