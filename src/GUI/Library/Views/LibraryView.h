/* LibraryView.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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
 *      Author: Lucio Carreras
 */

#ifndef MYTABLEVIEW_H_
#define MYTABLEVIEW_H_

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/SetFwd.h"

#include "GUI/Helper/SearchableWidget/SearchableTableView.h"
#include "GUI/Helper/Dragable/Dragable.h"
#include "GUI/InfoDialog/InfoDialogContainer.h"
#include "GUI/Library/Models/LibraryItemModel.h"

#include "Helper/MetaData/MetaDataFwd.h"
#include "Helper/Library/Sortorder.h"
#include "Helper/typedefs.h"
#include "Helper/Set.h"

#include <QAction>
#include <QMenu>
#include <QStringList>

class LibraryContextMenu;
class HeaderView;
class ColumnHeaderList;

class LibraryView :
		public SearchableTableView,
		public SayonaraClass,
		public InfoDialogContainer,
		private Dragable
{
	Q_OBJECT

signals:

	void sig_columns_changed(const BoolList&);

	void sig_middle_button_clicked(const QPoint&);
	void sig_all_selected();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();
	void sig_refresh_clicked();
	void sig_sortorder_changed(Library::SortOrder);

	void sig_no_disc_menu();
	void sig_import_files(const QStringList& files);
	void sig_double_clicked(const SP::Set<int>& indexes);
	void sig_sel_changed(const SP::Set<int>& indexes);
	void sig_merge(int target_id);


protected slots:
	virtual void header_actions_triggered(const BoolList& shown_cols);
	virtual void rc_menu_show(const QPoint&);
	virtual void sort_by_column(int);

	void merge_action_triggered();

	void language_changed();



public:
	explicit LibraryView(QWidget* parent=nullptr);
	virtual ~LibraryView();

	virtual void set_table_headers(const ColumnHeaderList& headers, const BoolList& shown_cols, Library::SortOrder sorting);

	virtual void save_selections();

	using QTableView::setModel;
	virtual void setModel(LibraryItemModel* model);
	virtual MetaDataList get_selected_metadata() const;

	void set_type(MD::Interpretation type);

	void show_rc_menu_actions(int entries);
	QMimeData* get_mimedata() const override;
	QPixmap get_pixmap() const override;




protected:
	// Events implemented in LibraryViewEvents.cpp
	virtual bool event(QEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dragMoveEvent(QDragMoveEvent *event) override;
	virtual void dropEvent(QDropEvent* event) override;

	virtual void resizeEvent(QResizeEvent *event) override;

	virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) override;
	virtual void rc_menu_init();

	HeaderView*	get_header_view();
	MD::Interpretation get_metadata_interpretation() const override;
	MetaDataList get_data_for_info_dialog() const override;



protected:

	LibraryItemModel*			_model=nullptr;

	QAction*					_merge_action=nullptr;
	QMenu*						_merge_menu=nullptr;

	LibraryContextMenu*			_rc_menu=nullptr;

	Library::SortOrder			_sort_order;

	bool						_cur_filling;
	MD::Interpretation			_type;


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

		for(int row=0; row < new_size; row++) 
		{
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
