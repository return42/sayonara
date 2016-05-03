/* PlaylistView.h */

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
 * PlaylistView.h
 *
 *  Created on: Jun 27, 2011
 *      Author: luke
 */

#ifndef PLAYLISTVIEW_H_
#define PLAYLISTVIEW_H_

#include "Components/Playlist/Playlist.h"
#include "GUI/Helper/SearchableWidget/SearchableListView.h"

#include <QPoint>
#include <QDrag>
#include <QList>
#include <QMenu>
#include <QPainter>
#include <QModelIndex>
#include <QMouseEvent>
#include <QScrollBar>


class LibraryContextMenu;
class PlaylistItemModel;
class PlaylistItemDelegate;
class PlaylistView : public SearchableListView
{

	Q_OBJECT

signals:
	void context_menu_emitted(const QPoint&);

	void sig_info_clicked();
	void sig_remove_clicked();
	void sig_edit_clicked();
	void sig_lyrics_clicked();

	void sig_double_clicked(int);
	void sig_no_focus();
	void sig_left_clicked();
	void sig_right_clicked();



public:
	PlaylistView(PlaylistPtr pl, QWidget* parent=nullptr);
	virtual ~PlaylistView();

	void set_context_menu_actions(int actions);

	void fill(PlaylistPtr pl);

	void scroll_up();
	void scroll_down();

	void dropEventFromOutside(QDropEvent* event);
	void set_current_track(int row);
	int get_num_rows();
	void remove_cur_selected_rows();


public slots:
	void clear();


private:
	QDrag*					_drag=nullptr;
	QPoint					_drag_pos;

	bool					_inner_drag_drop;

	LibraryContextMenu*		_rc_menu=nullptr;

	PlaylistItemModel*		_model=nullptr;
	PlaylistItemDelegate*	_delegate=nullptr;


private:

	void set_delegate_max_width(int n_items);
	void init_rc_menu();

	// Selections
	void goto_row(int row);


	// d & d
	void clear_drag_drop_lines(int row);
	int calc_drag_drop_line(QPoint pos);
	void handle_drop(QDropEvent* event, bool from_outside=false);

	// overloaded stuff
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dropEvent(QDropEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

	void keyPressEvent(QKeyEvent *event) override;
	void resizeEvent(QResizeEvent *e) override;
	void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) override;

	void init_shortcuts();


private slots:
	void row_pressed(const QModelIndex&);
	void row_double_clicked(const QModelIndex&);
	void row_released(const QModelIndex&);
	void _sl_look_changed();
};

#endif /* PlaylistView_H_ */
