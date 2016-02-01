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
#include "GUI/Helper/ComboBoxDelegate/ComboBoxDelegate.h"

#include <algorithm>

int SayonaraSelectionView::get_min_selected() const
{

	QItemSelectionModel* sel_model;
	QModelIndexList idx_list;

	sel_model = this->get_selection_model();
	if(!sel_model){
		return 0;
	}

	idx_list = sel_model->selectedRows();
	if(idx_list.isEmpty()) {
		return 0;
	}

	auto lambda_compare = [](const QModelIndex& idx1, const QModelIndex& idx2)
	{
		return idx1.row() < idx2.row();
	};

	auto it = std::min_element( idx_list.begin(), idx_list.end(), lambda_compare );

	return it->row();
}

void SayonaraSelectionView::select_all()
{
	QAbstractItemModel* model;
	QItemSelectionModel* sel_model;
	QItemSelection sel;

	model = this->get_model();
	sel_model = this->get_selection_model();
	if(!model || !sel_model){
		return;
	}

	sel = sel_model->selection();

	sel.select(model->index(0, 0), model->index(model->rowCount() - 1, model->columnCount() - 1));
	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}

void SayonaraSelectionView::select_rows(const IdxList& idx_list, int min_col, int max_col){

	QAbstractItemModel* model;
	QItemSelectionModel* sel_model;
	QItemSelection sel;

	model = this->get_model();
	sel_model = this->get_selection_model();
	if(!model || !sel_model){
		return;
	}

	if(idx_list.size() > 0){
		this->set_current_index(idx_list[0]);
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
	for(int i=0; i<idx_list.size(); i++){
		int j=i;

		do{
			j++;
			if(j == idx_list.size()){
				break;
			}

		} while(idx_list[j] - 1 == idx_list[j-1]);

		// select the range
		sel.select(model->index(idx_list[i], min_col),
				   model->index(idx_list[j-1], max_col)
		);
		i = j-1;
	}

	sel_model->select(sel, QItemSelectionModel::ClearAndSelect);
}

void SayonaraSelectionView::select_row(int row)
{
	QAbstractItemModel* model;
	QItemSelectionModel* sel_model;

	model = this->get_model();
	sel_model = this->get_selection_model();
	if(!model || !sel_model || model->rowCount() == 0){
		return;
	}

	row = std::min(model->rowCount() - 1, row);
	row = std::max(row, 0);

	sel_model->setCurrentIndex(model->index(row, 0), QItemSelectionModel::Select);

	select_rows(IdxList() << row, 0, 0);
}

void SayonaraSelectionView::clear_selection()
{
	QItemSelectionModel* sel_model;

	sel_model = this->get_selection_model();
	if(!sel_model){
		return;
	}

	sel_model->clearSelection();
}

IdxList SayonaraSelectionView::get_selections() const {

	IdxList idx_list_int;
	QItemSelectionModel* sel_model;
	QModelIndexList idx_list;

	sel_model = this->get_selection_model();
	if(!sel_model){
		return IdxList();
	}

	idx_list = sel_model->selectedRows();

	for(const QModelIndex& model_idx : idx_list) {
		idx_list_int << model_idx.row();
	}

	return idx_list_int;
}
