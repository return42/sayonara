/* SayonaraSelectionView.h */

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

#ifndef SAYONARASELECTIONVIEW_H
#define SAYONARASELECTIONVIEW_H

#include "Utils/typedefs.h"

#include <QModelIndex>

class QItemSelectionModel;

namespace SP
{
	template<typename T>
	class Set;
}

/**
 * @brief The SayonaraSelectionView class
 * @ingroup GUIInterfaces
 */
class SayonaraSelectionView
{

public:

	enum class SelectionType
	{
		Rows=0,
		Columns,
		Items
	};


protected:
	SelectionType _selection_type;

	virtual QItemSelectionModel* get_selection_model() const=0;
	virtual QModelIndex	get_index(int row, int col, const QModelIndex& parent=QModelIndex()) const=0;
	virtual int	get_row_count(const QModelIndex& parent=QModelIndex()) const=0;
	virtual int get_column_count(const QModelIndex& parent=QModelIndex()) const=0;
	virtual void set_current_index(int idx)=0;

    void select_rows(const IndexSet& indexes, int min_col=-1, int max_col=-1);
	void select_row(int row);

    void select_columns(const IndexSet& indexes, int min_row=-1, int max_row=-1);
	void select_column(int col);

    void select_items(const IndexSet& indexes);
	void select_item(int item);

	void select_all();

	void clear_selection();

	int get_min_selected_item() const;


public:
    IndexSet get_selected_items() const;

	virtual void set_selection_type(SayonaraSelectionView::SelectionType type);
	SayonaraSelectionView::SelectionType selection_type() const;

	virtual int get_index_by_model_index(const QModelIndex& idx) const=0;
	virtual QModelIndex get_model_index_by_index(int idx) const=0;

    virtual IndexSet get_indexes_by_model_indexes(const QModelIndexList& indexes) const;
    virtual QModelIndexList get_model_indexes_by_indexes(const IndexSet& indexes) const;

protected:
	SayonaraSelectionView();
	virtual ~SayonaraSelectionView();
};

#endif // SAYONARASELECTIONVIEW_H
