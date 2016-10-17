/* PlaylistViewEvents.cpp */

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



/* PlaylistViewEvents.cpp */

#include "PlaylistView.h"
#include "Helper/Set.h"
#include "Helper/MetaData/MetaData.h"
#include "GUI/Playlist/Delegate/PlaylistItemDelegate.h"
#include "GUI/Playlist/Model/PlaylistItemModel.h"
#include "GUI/Playlist/BookmarksMenu.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/CustomMimeData.h"

#include <QApplication>


void PlaylistView::contextMenuEvent(QContextMenuEvent* e)
{
	if(!_rc_menu){
		init_rc_menu();
	}

	QPoint pos = e->globalPos();
	QModelIndex idx = indexAt(e->pos());

	LibraryContexMenuEntries entry_mask = 0;

	if(this->get_num_rows() > 0)
	{
		entry_mask = (LibraryContextMenu::EntryClear);
	}

	SP::Set<int> selections = get_selections();
	if(selections.size() > 0)
	{
		entry_mask |=
				(LibraryContextMenu::EntryInfo |
				LibraryContextMenu::EntryRemove);
	}

	if(selections.size() == 1)
	{
		entry_mask |= (LibraryContextMenu::EntryLyrics);
	}

	if(_model->has_local_media(selections.toList()) )
	{
		entry_mask |= (LibraryContextMenu::EntryEdit);

		if(selections.size() == 1){

			MetaData md = _model->get_md(selections.first());
			_rc_menu->set_rating( md.rating );
			entry_mask |= LibraryContextMenu::EntryRating;
		}
	}

	_bookmarks_action->setVisible(
			idx.row() == _model->get_current_track() &&
			idx.row() >= 0 &&
			_bookmarks_menu->has_bookmarks());


	if(entry_mask > 0 || _bookmarks_action->isVisible()){
		_rc_menu->show_actions(entry_mask);
		_rc_menu->exec(pos);
	}

	SearchableListView::contextMenuEvent(e);
}


void PlaylistView::mousePressEvent(QMouseEvent* event)
{
	SearchableListView::mousePressEvent(event);

	if(event->buttons() & Qt::LeftButton){
		this->drag_pressed(event->pos());
	}
}


void PlaylistView::mouseMoveEvent(QMouseEvent* event)
{
	QDrag* drag = this->drag_moving(event->pos());
	if(drag){
		connect(drag, &QDrag::destroyed, this, [=]{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}

QMimeData* PlaylistView::get_mimedata() const
{
	CustomMimeData* mimedata = _model->get_custom_mimedata(this->selectedIndexes());
	if(!mimedata)
	{
		return nullptr;
	}

	mimedata->setObjectName("inner");
	return mimedata;
}

void PlaylistView::mouseDoubleClickEvent(QMouseEvent* event)
{
	SearchableListView::mouseDoubleClickEvent(event);

	QModelIndex idx = this->indexAt(event->pos());

	if(idx.isValid()){
		_model->set_current_track(idx.row());
	}

	if( (idx.flags() & Qt::ItemIsEnabled) && 
		(idx.flags() & Qt::ItemIsSelectable))
	{
		emit sig_double_clicked(idx.row());
	}
	
}

void PlaylistView::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();

	if((key == Qt::Key_Up || key == Qt::Key_Down)) {

		SP::Set<int> selections = this->get_selections();
		bool ctrl_pressed = (event->modifiers() & Qt::ControlModifier);

		if( ctrl_pressed && !selections.isEmpty() )
		{
			SP::Set<int> new_selections;

			int min_row = *selections.begin();
			int max_row = *selections.rbegin();

			if(key == Qt::Key_Up){
				if(min_row == 0){
					return;
				}
				_model->move_rows(selections, min_row - 1);
				for(int i=min_row - 1; i<(int) selections.size() + min_row - 1; i++){
					new_selections.insert(i);
				}
			}

			else{
				if(max_row >= _model->rowCount() - 1){
					return;
				}
				_model->move_rows(selections, max_row + 2);
				for(int i=min_row + 1; i<(int) selections.size() + min_row + 1; i++){
					new_selections.insert(i);
				}
			}

			if(new_selections.size() > 0){
				this->select_rows(new_selections);
			}
		}

		else if(selections.isEmpty()) {
			if(_model->rowCount() > 0) {
				if(key == Qt::Key_Up){
					select_row(_model->rowCount() - 1);
				}
				else{
					select_row(0);
				}
			}
			return;
		}
	}

	if(event->matches(QKeySequence::SelectAll)){
		select_all();
		return;
	}

	else if(event->matches(QKeySequence::Delete)){
		this->remove_cur_selected_rows();
		return;
	}

	SearchableListView::keyPressEvent(event);

	if(!event->isAccepted() ) {
		return;
	}

	int new_row = -1;
	int min_row = get_min_selected();

	switch(key) {
		case Qt::Key_End:
			new_row = _model->rowCount() - 1;
			break;

		case Qt::Key_Home:
			new_row = 0;
			break;

		case Qt::Key_Left:
			if(event->modifiers() & Qt::ControlModifier){
				emit sig_left_tab_clicked();
			}
			break;

		case Qt::Key_Right:
			if(event->modifiers() & Qt::ControlModifier){
				emit sig_right_tab_clicked();
			}
			break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
			if(min_row >= 0){
				_model->set_current_track(min_row);
				emit sig_double_clicked(min_row);
			}

			break;

		default:
			break;
	}

	if(new_row != -1) {
		goto_row(new_row);
	}
}


void PlaylistView::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept();
}

void PlaylistView::dragMoveEvent(QDragMoveEvent* event)
{
	event->accept();

	int first_row = this->indexAt(QPoint(5, 5)).row();
	int last_row = this->indexAt(QPoint(5, this->height())).row() - 1;
	int row = calc_drag_drop_line(event->pos() );

	bool is_old = _delegate->is_drag_index(row);

	if(!is_old){
		clear_drag_drop_lines(_delegate->get_drag_index());
		_delegate->set_drag_index(row);
		this->update(_model->index(row));
	}

	if(row == first_row){
		scroll_up();
	}
	if(row == last_row){
		scroll_down();
	}
}

void PlaylistView::dragLeaveEvent(QDragLeaveEvent* event)
{
	event->accept();
	clear_drag_drop_lines(_delegate->get_drag_index());
}

void PlaylistView::dropEventFromOutside(QDropEvent* event)
{
	event->accept();
	handle_drop(event);
}


void PlaylistView::dropEvent(QDropEvent* event)
{
	event->accept();
	handle_drop(event);
}


void PlaylistView::resizeEvent(QResizeEvent *e) {

	SearchableListView::resizeEvent(e);

	this->set_delegate_max_width(_model->rowCount());
}

