/* SearchableListView.h */

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



#ifndef SEARCHABLELISTVIEW_H
#define SEARCHABLELISTVIEW_H


#include "SayonaraSelectionView.h"

#include <QListView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QModelIndexList>

class Settings;
class MiniSearcher;
class SearchModelInterface;


class SearchableListView :
		public QListView,
		public SayonaraSelectionView
{
	Q_OBJECT

private slots:
	void edit_changed(const QString& str);
	void fwd_clicked();
	void bwd_clicked();

	void search_mode_changed();

private:

	enum class SearchDirection : quint8
	{
		First,
		Next,
		Prev
	};

	MiniSearcher*					_mini_searcher=nullptr;
	SearchModelInterface*			_abstr_model=nullptr;
	Settings*						_settings=nullptr;
	int								_cur_row;


private:
	QItemSelectionModel* get_selection_model() const override;

	QModelIndex get_index(int row, int col) const override;
	int get_row_count() const override;
	int get_column_count() const override;

	QModelIndex get_match_index(const QString& str, SearchDirection direction) const;
	void select_match(const QString& str, SearchDirection direction);


public:
	explicit SearchableListView(QWidget* parent=nullptr);
	virtual ~SearchableListView();

	void setSearchModel(SearchModelInterface* model);

protected:
	void keyPressEvent(QKeyEvent *) override;

	void set_current_index(int idx);
};

#endif
