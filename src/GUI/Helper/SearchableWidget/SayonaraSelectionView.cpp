/* SayonaraSelectionView.cpp */

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

#include "SayonaraSelectionView.h"
#include "Helper/Set.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include <algorithm>

SayonaraSelectionView::SayonaraSelectionView(){}

SayonaraSelectionView::~SayonaraSelectionView(){}

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
	SP::Set<int> indexes;
	indexes.insert(row);
	select_rows(indexes);
}

void SayonaraSelectionView::clear_selection()
{
	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	sel_model->clearSelection();
}

SP::Set<int> SayonaraSelectionView::get_selections() const
{


	QItemSelectionModel* sel_model = this->get_selection_model();
	if(!sel_model){
		return SP::Set<int>();
	}

	QModelIndexList idx_list = sel_model->selectedRows();

	SP::Set<int> indexes;
	for(const QModelIndex& model_idx : idx_list) {
		indexes.insert(model_idx.row());
	}

	return indexes;
}


int SayonaraSelectionView::get_min_selected() const
{
	SP::Set<int> selected = this->get_selections();
	auto it = std::min_element(selected.begin(), selected.end());
	if(it != selected.end()){
		return *it;
	}

	return -1;
}
