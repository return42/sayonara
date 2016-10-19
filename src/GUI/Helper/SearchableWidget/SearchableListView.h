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
#include "Helper/Library/SearchMode.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QListView>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QModelIndexList>

class Settings;
class AbstractSearchListModel;
class MiniSearcher;

class AbstractSearchListModel;
class SearchableListView :
		public QListView,
		public SayonaraSelectionView,
		protected SayonaraClass
{
	Q_OBJECT

signals:
	void sig_mouse_moved();
	void sig_mouse_pressed();
	void sig_mouse_released();
	void sig_focus_out();
	void sig_key_pressed(QKeyEvent*);
	void sig_selection_changed(const QModelIndexList& idxs);


private slots:
	void edit_changed(const QString& str);
	void fwd_clicked();
	void bwd_clicked();

private:
	MiniSearcher*               _mini_searcher=nullptr;
	AbstractSearchListModel*	_abstr_model=nullptr;
	int							_cur_row;
	Library::SearchModeMask		_search_mode;

private:
	virtual QAbstractItemModel* get_model() const override;
	virtual QItemSelectionModel* get_selection_model() const override;
	virtual void set_current_index(int idx) override;

private slots:
	void search_mode_changed();

public:
	explicit SearchableListView(QWidget* parent=nullptr);
	virtual ~SearchableListView();

	void setAbstractModel(AbstractSearchListModel* model);
	Library::SearchModeMask search_mode() const;

protected:
	void mouseMoveEvent(QMouseEvent *) override;
	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void keyPressEvent(QKeyEvent *) override;
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) override;
};

#endif // SEARCHABLELISTVIEW_H
