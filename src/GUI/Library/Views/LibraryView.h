/* LibraryView.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * MyListView.h
 *
 *  Created on: Jun 26, 2011
 *      Author: luke
 */

#ifndef MYTABLEVIEW_H_
#define MYTABLEVIEW_H_

#include "GUI/Library/ColumnHeader.h"
#include "GUI/Helper/SearchableWidget/SearchableTableView.h"
#include "GUI/Library/Models/LibraryItemModel.h"

#include "Helper/MetaData/MetaDataList.h"

#include <QDropEvent>
#include <QMouseEvent>
#include <QStringList>
#include <QHeaderView>
#include <QAction>
#include <QIcon>
#include <QLineEdit>
#include <QScrollBar>
#include <QFont>
#include <QDrag>
#include <QApplication>
#include <QMenu>

class LibraryItemModel;
class LibraryContextMenu;
class MiniSearcher;
class CustomMimeData;

class LibraryView : public SearchableTableView{

	Q_OBJECT

signals:

	void sig_columns_changed(const BoolList&);

	void sig_middle_button_clicked(const QPoint&);
	void sig_info_clicked();
	void sig_edit_clicked();
	void sig_all_selected();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();
	void sig_refresh_clicked();
	void sig_sortorder_changed(SortOrder);

	void sig_no_disc_menu();
	void sig_import_files(const QStringList&);
	void sig_double_clicked(const SP::Set<int>&);
	void sig_sel_changed(const SP::Set<int>&);


protected slots:
	virtual void rc_header_menu_changed(bool b=true);
	virtual void rc_menu_show(const QPoint&);
	virtual void sort_by_column(int);


public:
	LibraryView(QWidget* parent=nullptr);
	virtual ~LibraryView();

	virtual void rc_header_menu_init(const BoolList& lst);
	virtual void rc_header_menu_retranslate();
	virtual void set_table_headers(const QList<ColumnHeader>& headers, SortOrder sorting);

	virtual void save_selections();

	using QTableView::setModel;
	virtual void setModel(LibraryItemModel* model);

	// entries are specified in ContextMenu.h
	virtual void set_rc_menu(int entries);

	virtual MetaDataList get_selected_metadata() const;


protected:
	// Events implemented in LibraryViewEvents.cpp
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dragMoveEvent(QDragMoveEvent *event) override;
	virtual void resizeEvent(QResizeEvent *event) override;

	virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
	virtual void rc_menu_init();

	virtual void do_drag();


	virtual void set_col_sizes();

protected:

	LibraryItemModel*			_model=nullptr;

	QDrag*						_drag=nullptr;
	QPoint						_drag_pos;

	QMenu*						_rc_header_menu=nullptr;
	LibraryContextMenu*			_rc_menu=nullptr;

	QList<ColumnHeader>			_table_headers;
	QList<QAction*>				_header_rc_actions;

	SortOrder					_sort_order;

	bool						_cur_filling;


public:
	template < typename T, typename ModelType >
	void fill(const T& input_data){

		SP::Set<int> indexes;
		int old_size, new_size;

		clearSelection();

		_cur_filling = true;

		old_size = _model->rowCount();
		new_size = input_data.size();

		if(old_size > new_size){
			_model->removeRows(new_size, old_size - new_size);
		}
		else if(old_size < new_size){
			_model->insertRows(old_size, new_size - old_size);
		}

		for(int row=0; row < new_size; row++) {

			if(_model->is_selected(input_data[row].id)){
				indexes.insert(row);
			}
		}

		QModelIndex idx = _model->index(0, 0);

		ModelType* model = static_cast<ModelType*>(_model);
		model->setData(idx, input_data, Qt::DisplayRole);

		_model->clear_selections();

		select_rows(indexes, 0, _model->columnCount() - 1);

		_cur_filling = false;
	}
};

#endif /* MYLISTVIEW_H_ */
