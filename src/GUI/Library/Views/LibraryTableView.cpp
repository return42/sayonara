/* LibraryTableView.cpp */

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



#include "LibraryTableView.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Views/HeaderView.h"
#include "Helper/Settings/Settings.h"
#include <algorithm>

template <typename T>
void switch_sorters(T& srcdst, T src1, T src2)
{
	if(srcdst == src1) {
		srcdst = src2;
	}

	else {
		srcdst = src1;
	}
}

struct LibraryTableView::Private
{
	Library::SortOrder sort_order;
};

LibraryTableView::LibraryTableView(QWidget* parent) :
	LibraryView(parent)
{
	m = Pimpl::make<Private>();

	HeaderView* header = new HeaderView(Qt::Horizontal, this);
	this->setHorizontalHeader(header);

	connect(header, &HeaderView::sectionClicked, this, &LibraryTableView::sort_by_column);
	connect(header, &HeaderView::sig_columns_changed, this, &LibraryTableView::header_actions_triggered);

	REGISTER_LISTENER_NO_CALL(Set::Player_Language, language_changed);
}

LibraryTableView::~LibraryTableView() {}

void LibraryTableView::set_table_headers(
		const ColumnHeaderList& headers, const BoolList& shown_cols, Library::SortOrder sorting)
{
	HeaderView* header_view = this->get_header_view();

	_model->removeColumns(0, _model->columnCount());
	_model->insertColumns(0, headers.size());

	int i=0;
	for(ColumnHeader* header : headers){
		_model->setHeaderData(i, Qt::Horizontal, header->get_title(), Qt::DisplayRole);
		i++;
	}

	header_view->set_column_headers(headers, shown_cols, sorting);

	language_changed();
}


HeaderView* LibraryTableView::get_header_view()
{
	return dynamic_cast<HeaderView*>(this->horizontalHeader());
}


void LibraryTableView::header_actions_triggered(const BoolList& shown_cols)
{
	SP::Set<int> sel_indexes = get_selected_items();

	std::for_each(sel_indexes.begin(), sel_indexes.end(), [this](int row){
		this->selectRow(row);
	});

	emit sig_columns_changed(shown_cols);
}

void LibraryTableView::sort_by_column(int column_idx)
{
	Library::SortOrder asc_sortorder, desc_sortorder;

	HeaderView* header_view = this->get_header_view();

	int idx_col = header_view->visualIndex(column_idx);
	ColumnHeader* h = header_view->get_column_header(idx_col);
	if(!h){
		return;
	}

	asc_sortorder = h->get_asc_sortorder();
	desc_sortorder = h->get_desc_sortorder();

	switch_sorters( m->sort_order, asc_sortorder, desc_sortorder );

	emit sig_sortorder_changed(m->sort_order);
}



void LibraryTableView::language_changed()
{
	HeaderView* header_view = get_header_view();

	for(int i=0; i<_model->columnCount(); i++){
		ColumnHeader* header = header_view->get_column_header(i);
		if(header){
			_model->setHeaderData(i, Qt::Horizontal, header->get_title(), Qt::DisplayRole);
		}
	}
}



void LibraryTableView::resizeEvent(QResizeEvent* event)
{
	LibraryView::resizeEvent(event);
	get_header_view()->refresh_sizes(this);
}


int LibraryTableView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex LibraryTableView::get_model_index_by_index(int idx) const
{
	return _model->index(idx, 0);
}
