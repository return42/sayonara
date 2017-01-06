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

#include <QList>
#include <QItemSelectionModel>
#include <QAbstractItemModel>

class QModelIndex;

namespace SP
{
	template<typename T>
	class Set;
}

class SayonaraSelectionView
{

protected:
	virtual QItemSelectionModel* get_selection_model() const=0;
	virtual QModelIndex	get_index(int row, int col, const QModelIndex& parent=QModelIndex()) const=0;
	virtual int	get_row_count(const QModelIndex& parent=QModelIndex()) const=0;
	virtual int get_column_count(const QModelIndex& parent=QModelIndex()) const=0;
	virtual void set_current_index(int idx)=0;

	void select_all();
	void select_rows(const SP::Set<int>& indexes, int min_col=-1, int max_col=-1);
	void select_row(int row);
	void clear_selection();
	int get_min_selected() const;

public:
	SP::Set<int> get_selections() const;

protected:
	SayonaraSelectionView();
	virtual ~SayonaraSelectionView();
};


#endif // SAYONARASELECTIONVIEW_H
