/* LibraryViewHeader.cpp */

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

#include "GUI/Library/Views/LibraryView.h"
#include "GUI/Library/Helper/ColumnHeader.h"

#include <QHeaderView>
#include <QMouseEvent>

template <typename T>
void switch_sorters(T& srcdst, T src1, T src2) {
	if(srcdst == src1) {
		srcdst = src2;
	}

	else {
		srcdst = src1;
	}
}


void LibraryView::sort_by_column(int col) {


	QHeaderView* header = this->horizontalHeader();
	SortOrder asc_sortorder, desc_sortorder;
	int idx_col;

	idx_col = header->visualIndex(col);
	if(idx_col >= _table_headers.size()) {
		return;
	}

	ColumnHeader h = _table_headers[idx_col];
	asc_sortorder = h.get_asc_sortorder();
	desc_sortorder = h.get_desc_sortorder();

	switch_sorters( _sort_order, asc_sortorder, desc_sortorder );

	emit sig_sortorder_changed(_sort_order);
}


void LibraryView::set_table_headers(const QList<ColumnHeader>& headers, SortOrder sorting) {

	_table_headers = headers;

	QHeaderView* header_view = this->horizontalHeader();
	int i=0;

	for(const ColumnHeader& header : headers){

		if( header.get_asc_sortorder() == sorting) {
			header_view->setSortIndicator(i, Qt::AscendingOrder);
			_sort_order = sorting;
			break;
		}

		else if( header.get_desc_sortorder() == sorting) {
			header_view->setSortIndicator(i, Qt::DescendingOrder);
			_sort_order = sorting;
			break;
		}

		i++;
	}
}


void LibraryView::rc_header_menu_init(const BoolList& shown_cols) {

	if(_rc_header_menu) {
		delete _rc_header_menu;
	}

	QHeaderView* header_view;
	Qt::SortOrder asc;
	int col_idx;


	int i=0;
	bool show_sorter = true;

	header_view = this->horizontalHeader();

	_rc_header_menu = new QMenu( header_view );

	// in this moment all columns are still shown

	header_view = this->horizontalHeader();
	col_idx = header_view->sortIndicatorSection();
	asc = header_view->sortIndicatorOrder();

	for(const ColumnHeader& header : _table_headers) {

		QAction* action = new QAction(header.getTitle(), this);
		bool switchable = header.getSwitchable();

		action->setCheckable(true);
		action->setEnabled(switchable);

		if( !switchable ) {
			action->setChecked(true);
		}

		else if(i >= shown_cols.size()){
			action->setChecked(false);
		}

		else{

			bool checked = shown_cols[i];
			action->setChecked(checked);

			// where should we put the sorters?
			// if a few columns are missing before the origin position,
			// the index of the sorted column has to be decreased
			if(!checked){

				if(i<col_idx) {
					col_idx --;
				}

				else if(i == col_idx){
					show_sorter = false;
				}
			}
		}

		connect(action, &QAction::toggled, this, &LibraryView::rc_header_menu_changed);

		_header_rc_actions << action;
		header_view->addAction(action);
		i++;
	}

	rc_header_menu_changed();

	if(show_sorter) {
		header_view->setSortIndicator(col_idx, asc);
	}

	header_view->setContextMenuPolicy(Qt::ActionsContextMenu);
}


void LibraryView::rc_header_menu_retranslate(){
	QStringList header_names = _model->get_header_names();

	int i=0;
	for(const QString& header_name : header_names){
		if(i >= _header_rc_actions.size()){
			sp_log(Log::Warning) << "Cannot update languages in library headers";
			break;
		}

		_header_rc_actions[i]->setText(header_name);
		i++;
	}
}



void LibraryView::rc_header_menu_changed(bool b) {

	Q_UNUSED(b);

	int col_idx = 0;
	SP::Set<int> sel_indexes = get_selections();
	BoolList lst;


	_model->removeColumns(0, _model->columnCount());

	for(const QAction* action : _header_rc_actions) {

		if(action->isChecked()) {
			_model->insertColumn(col_idx);
			lst << true;
		}

		else {
			lst << false;
		}

		col_idx++;
	}

	emit sig_columns_changed(lst);
	set_col_sizes();

	std::for_each(sel_indexes.begin(), sel_indexes.end(), [this](int row){
		this->selectRow(row);
	});
}


void LibraryView::set_col_sizes() {

	QHeaderView* header = this->horizontalHeader();

	int altogether_width = 0;
	int desired_width = 0;
	int tolerance = 30;
	double altogether_percentage = 0;
	int n_cols = _model->columnCount();


	for(int i=0; i<n_cols; i++) {

		int col = header->visualIndex(i);

		int preferred_size = 0;

		ColumnHeader h = _table_headers[col];

		if(h.getSizeType() == ColHeaderSizeAbs) {

			preferred_size = h.get_preferred_size_abs();
		}

		else{

			altogether_percentage += h.get_preferred_size_rel();
			desired_width += h.get_preferred_size_abs();
		}

		altogether_width += preferred_size;
	}

	altogether_width += tolerance;

	int target_width = this->width() - altogether_width;


	if(target_width < desired_width) {
		target_width = desired_width;
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}

	else{
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	}

	// width for percentage stuff
	for(int i=0; i<n_cols; i++) {

		int col = header->visualIndex(i);
		int preferred_size = 0;

		ColumnHeader h = _table_headers[col];
		if(h.getSizeType() == ColHeaderSizeRel) {
			preferred_size = (h.get_preferred_size_rel() / altogether_percentage) * target_width;
		}

		else{
			preferred_size = h.get_preferred_size_abs();
		}

		this->setColumnWidth(i, preferred_size);
	}
}

// header end
