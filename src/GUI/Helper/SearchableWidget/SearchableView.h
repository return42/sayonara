/* SearchableView.h */

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

#ifndef SEARCHABLEVIEW_H
#define SEARCHABLEVIEW_H

#include "GUI/Helper/SearchableWidget/SayonaraSelectionView.h"
#include "Helper/Pimpl.h"

#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QKeyEvent>

class QItemSelectionModel;
class SearchModelFunctionality;

/**
 * @brief The SearchViewInterface class
 * @ingroup GUIInterfaces
 */
class SearchViewFunctionality :
		public SayonaraSelectionView
{
	PIMPL(SearchViewFunctionality)

protected:
	enum class SearchDirection : unsigned char
	{
		First,
		Next,
		Prev
	};

public:
	explicit SearchViewFunctionality(QAbstractItemView* view);
	virtual ~SearchViewFunctionality();

	virtual void setSearchModel(SearchModelFunctionality* model) final;

	virtual QModelIndex get_index(int row, int col, const QModelIndex& parent=QModelIndex()) const override final;
	virtual int get_row_count(const QModelIndex& parent=QModelIndex()) const override final;
	virtual int get_column_count(const QModelIndex& parent=QModelIndex()) const override final;

	virtual QItemSelectionModel* get_selection_model() const override final;
	virtual void set_current_index(int idx) override final;

private:
	QModelIndex get_match_index(const QString& str, SearchDirection direction) const;
	void select_match(const QString& str, SearchDirection direction);

protected:
	void handleKeyPress(QKeyEvent* e);
};


template<typename AbstractView>
class SearchViewInterface :
		public AbstractView,
		public SearchViewFunctionality
{
public:
	SearchViewInterface(QWidget* parent=nullptr) :
		AbstractView(parent),
		SearchViewFunctionality(this) {}

	virtual ~SearchViewInterface() {}

protected:
	void keyPressEvent(QKeyEvent* e) override
	{
		handleKeyPress(e);
	}
};

typedef SearchViewInterface<QTableView> SearchableTableView;
typedef SearchViewInterface<QListView> SearchableListView;
typedef SearchViewInterface<QTreeView> SearchableTreeView;

#endif // SEARCHABLEVIEW_H
