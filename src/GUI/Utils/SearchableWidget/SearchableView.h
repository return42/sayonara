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

#include "GUI/Utils/SearchableWidget/SayonaraSelectionView.h"
#include "Utils/Pimpl.h"

#include <QKeyEvent>
#include <QTableView>
#include <QListView>
#include <QTreeView>

class QAbstractItemView;
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

		virtual QModelIndex model_index(int row, int col, const QModelIndex& parent=QModelIndex()) const override final;
		virtual int row_count(const QModelIndex& parent=QModelIndex()) const override final;
		virtual int column_count(const QModelIndex& parent=QModelIndex()) const override final;

		virtual QItemSelectionModel* selection_model() const override final;
		virtual void set_current_index(int idx) override final;

		bool is_minisearcher_active() const;
		void set_mini_searcher_padding(int padding);

	private:
		QModelIndex match_index(const QString& str, SearchDirection direction) const;
		void select_match(const QString& str, SearchDirection direction);

	protected:
		void handle_key_press(QKeyEvent* e) override;

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
		if(!e->isAccepted()){
			handle_key_press(e);
			if(e->isAccepted()){
				return;
			}
		}

		AbstractView::keyPressEvent(e);
	}
};

using SearchableTableView=SearchViewInterface<QTableView>;
using SearchableListView=SearchViewInterface<QListView>;
using SearchableTreeView=SearchViewInterface<QTreeView>;

#endif // SEARCHABLEVIEW_H
