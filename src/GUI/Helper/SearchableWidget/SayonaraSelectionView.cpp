/* SayonaraSelectionView.cpp */

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

#include "SayonaraSelectionView.h"
#include "Helper/Set.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include <algorithm>

SayonaraSelectionView::SayonaraSelectionView()
{
	_selection_type = SayonaraSelectionView::SelectionType::Rows;
}

SayonaraSelectionView::~SayonaraSelectionView() {}

void SayonaraSelectionView::select_all()
{
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	int n_rows = get_row_count();
	int n_cols = get_column_count();

	QModelIndex first_idx = get_index(0, 0);
	QModelIndex last_idx = get_index(n_rows - 1, n_cols - 1);

	QItemSelection sel = sel_model->selection();
	sel.select(first_idx, last_idx);
	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}



void SayonaraSelectionView::select_rows(const SP::Set<int>& indexes, int min_col, int max_col)
{
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	if(indexes.size() > 0) {
		int first_index = indexes.first();
		this->set_current_index(first_index);
	}

	min_col = std::max(0, min_col);
	min_col = std::min(min_col, get_column_count() - 1);
	max_col = std::max(0, max_col);
	max_col = std::min(max_col, get_column_count() - 1);

	QItemSelection sel;
	if(indexes.size() == 1)
	{
		QModelIndex first_idx = get_index(indexes.first(), 0);
		QModelIndex last_idx = get_index(indexes.first(), get_column_count() - 1);

		sel.select(first_idx, last_idx);
		sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
		return;
	}


	// the goal is: find consecutive ranges.
	// For every select or merge an overlap is
	// tested, that will last really long when
	// there are already around 500 items.
	// probably, the runtime is O(n^2)

	// the list is pre-sorted (see the fill-function)

	// we start at the very beginning (i)
	// let j run until the first element that is not
	// consecutive.

	// our range is from i to the last known j.
	// count down the j by one. So the worst that can
	// happen, is that j is as big as i again.
	// i is increased in the next loop, so progress is
	// guaranteed


	for(auto it=indexes.begin(); it!=indexes.end(); it++)
	{
		auto other_it=it;
		auto other_predecessor=it;

		do
		{
			other_predecessor = other_it;
			other_it++;

			if(other_it == indexes.end()){
				break;
			}

		} while(*other_it - 1 == *other_predecessor);

		// select the range

		QModelIndex min_idx = get_index(*it, min_col);
		QModelIndex max_idx = get_index(*other_predecessor, max_col);
		sel.select(min_idx, max_idx);

		it = other_it;

		if(it == indexes.end()){
			break;
		}
	}

	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}


void SayonaraSelectionView::select_row(int row)
{
	SP::Set<int> indexes(row);
	select_rows(indexes);
}

void SayonaraSelectionView::select_columns(const SP::Set<int>& indexes, int min_row, int max_row)
{
	//TODO
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	QItemSelection sel;
	for(auto it = indexes.begin(); it != indexes.end(); it++){
		sel.select(get_index(min_row, *it),
				   get_index(max_row, *it));
	}

	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}

void SayonaraSelectionView::select_column(int col)
{
	SP::Set<int> indexes(col);
	select_columns(col);
}

void SayonaraSelectionView::select_items(const SP::Set<int>& items)
{
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	QItemSelection sel;
	for(auto it = items.begin(); it != items.end(); it++){
		sel.select( get_model_index_by_index(*it), get_model_index_by_index(*it) );
	}

	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}

void SayonaraSelectionView::clear_selection()
{
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	sel_model->clearSelection();
}


SP::Set<int> SayonaraSelectionView::get_selected_items() const
{
	SP::Set<int> indexes;
	QItemSelectionModel* sel_model = this->get_selection_model();

	if(!sel_model){
		return indexes;
	}

	QModelIndexList idx_list = sel_model->selectedIndexes();

	for(const QModelIndex& model_idx : idx_list) {
		indexes.insert( get_index_by_model_index(model_idx) );
	}

	return indexes;
}


SP::Set<int> SayonaraSelectionView::get_indexes_by_model_indexes(const QModelIndexList& idxs) const
{
	SP::Set<int> ret;
	for(const QModelIndex& idx : idxs){
		ret.insert( get_index_by_model_index(idx) );
	}
	return ret;
}


QModelIndexList SayonaraSelectionView::get_model_indexes_by_indexes(const SP::Set<int>& idxs) const
{
	QModelIndexList lst;
	for(auto it = idxs.begin(); it != idxs.end(); it++){
		lst << get_model_index_by_index(*it);
	}
	return lst;
}



int SayonaraSelectionView::get_min_selected_item() const
{
	SP::Set<int> selected = get_selected_items();
	auto it = std::min_element(selected.begin(), selected.end());
	if(it != selected.end()){
		return *it;
	}

	return -1;
}


void SayonaraSelectionView::set_selection_type(SayonaraSelectionView::SelectionType type)
{
	_selection_type = type;
}

SayonaraSelectionView::SelectionType SayonaraSelectionView::selection_type() const
{
	return _selection_type;
}

