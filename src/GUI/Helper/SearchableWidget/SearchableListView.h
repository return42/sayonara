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

#include "SearchableListView.h"

#include "AbstractSearchModel.h"
#include "SayonaraSelectionView.h"

#include "GUI/Library/Models/LibraryItemModel.h"

#include <QListView>
#include <QMouseEvent>
#include <QKeyEvent>

class MiniSearcher;
class SearchableListView : public QListView, public SayonaraSelectionView {
	Q_OBJECT

signals:
	void sig_mouse_moved();
	void sig_mouse_pressed();
	void sig_mouse_released();
	void sig_focus_out();
	void sig_key_pressed(QKeyEvent*);

private slots:
	void edit_changed(const QString& str);
	void fwd_clicked();
	void bwd_clicked();

private:

	MiniSearcher*               _mini_searcher;
	AbstractSearchListModel*	_abstr_model;
	int							_cur_row;

	virtual void set_current_index(int idx) override;
	virtual QAbstractItemModel* get_model() const override;
	virtual QItemSelectionModel* get_selection_model() const override;

public:
	SearchableListView(QWidget* parent=nullptr);
	virtual ~SearchableListView();
	void setAbstractModel(AbstractSearchListModel* model);

protected:
	virtual void mouseMoveEvent(QMouseEvent *) override;
	virtual void mousePressEvent(QMouseEvent *) override;
	virtual void mouseReleaseEvent(QMouseEvent *) override;
	virtual void keyPressEvent(QKeyEvent *) override;
};


#endif // SEARCHABLELISTVIEW_H
