/* PlaylistView.cpp */

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
 * PlaylistView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: Lucio Carreras
 */

#include "PlaylistView.h"
#include "GUI/Playlist/Model/PlaylistItemModel.h"
#include "GUI/Playlist/Delegate/PlaylistItemDelegate.h"
#include "GUI/Playlist/PlaylistContextMenu.h"
#include "GUI/Helper/CustomMimeData.h"

#include "Helper/Helper.cpp"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Components/Playlist/PlaylistHandler.h"

#include <QApplication>
#include <QShortcut>

PlaylistView::PlaylistView(PlaylistPtr pl, QWidget* parent) :
	SearchableListView(parent)
{
	_model = new PlaylistItemModel(pl, this);
	_delegate = new PlaylistItemDelegate(this);

	_drag = nullptr;
	_inner_drag_drop = false;

	init_rc_menu();

	this->setModel(_model);
	this->setAbstractModel(_model);
	this->setItemDelegate(_delegate);
	this->setDragEnabled(true);
	this->setDropIndicatorShown(true);
	this->setDragDropMode(QAbstractItemView::DragDrop);
	this->setAcceptDrops(true);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	this->setAlternatingRowColors(true);
	this->setMovement(QListView::Free);

	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	new QShortcut(QKeySequence(Qt::Key_Backspace), this, SLOT(clear()), nullptr, Qt::WidgetShortcut);

	connect(this, &PlaylistView::pressed, this, &PlaylistView::row_pressed);
	connect(this, &PlaylistView::doubleClicked, this, &PlaylistView::row_double_clicked);
	connect(this, &PlaylistView::clicked, this, &PlaylistView::row_released);

	REGISTER_LISTENER(Set::PL_EntryLook, _sl_look_changed);
}

PlaylistView::~PlaylistView() {

	if(_rc_menu){
		delete _rc_menu;
	}

	delete _model;
	delete _delegate;
}


void PlaylistView::mousePressEvent(QMouseEvent* event) {

	if(_model->rowCount() == 0){
		return;
	}

	QPoint pos_org;
	QPoint pos;
	SP::Set<int> selections;

	switch (event->button()) {

		case Qt::LeftButton:

			SearchableListView::mousePressEvent(event);

			if(!this->indexAt(event->pos()).isValid()){
				_drag_pos = QPoint();
			}

			else{
				_drag_pos = event->pos();
			}


			break;

		case Qt::RightButton:

			LibraryContexMenuEntries entry_mask;
			SearchableListView::mousePressEvent(event);

			pos_org = event->pos();
			pos = QWidget::mapToGlobal(pos_org);

			pos.setY(pos.y());
			pos.setX(pos.x() + 10);

			entry_mask = (LibraryContextMenu::EntryInfo |
						  LibraryContextMenu::EntryRemove |
						  LibraryContextMenu::EntryClear);


			selections = get_selections();
			if(selections.size() == 1){
				entry_mask |= LibraryContextMenu::EntryLyrics;
			}

			if(_model->has_local_media(selections.toList()) ){
				entry_mask |= LibraryContextMenu::EntryEdit;
			}

			set_context_menu_actions(entry_mask);

			_rc_menu->exec(pos);

			break;

		default:
			break;
	}
}


void PlaylistView::mouseMoveEvent(QMouseEvent* event) {

	int distance = (event->pos() - _drag_pos).manhattanLength();
	QModelIndex idx = this->indexAt(event->pos());

	if(!idx.isValid()){
		return;
	}

	if( event->buttons() & Qt::LeftButton &&
			distance >= QApplication::startDragDistance())
	{

		CustomMimeData* mimedata;

		if(_drag_pos.isNull()){
			return;
		}

		if(_drag){
			delete _drag;
			_drag = nullptr;
		}

		if(_model->rowCount() == 0){
			return;
		}

		mimedata = _model->get_custom_mimedata(this->selectedIndexes());
		if(!mimedata){
			return;
		}

		_drag = new QDrag(this);

		connect(_drag, &QDrag::destroyed, [=](){
			_drag = nullptr;
		});

		_drag->setMimeData(mimedata);
		_drag->exec(Qt::CopyAction);
	}

	else if(event->buttons() == 0){
		MetaData md = _model->get_md(idx.row());
		QString tooltip_string = "<b>" + md.title + "</b>\n" + md.artist;
		this->setToolTip(tooltip_string);
	}
}


void PlaylistView::mouseReleaseEvent(QMouseEvent* event) {

	switch (event->button()) {

		case Qt::LeftButton:

			SearchableListView::mouseReleaseEvent(event);
			event->accept();

			break;

		default:
			break;
	}
}


// mark row as currently pressed
void PlaylistView::goto_row(int row) {

	if( (row >= _model->rowCount()) || (row < 0) ) {
		return;
	}

	QModelIndex idx = _model->index(row, 0);
	row_released(idx);

	this->scrollTo(idx, SearchableListView::EnsureVisible);
}


void PlaylistView::keyPressEvent(QKeyEvent* event) {

	int key = event->key();

	if((key == Qt::Key_Up || key == Qt::Key_Down)) {
		if(this->selectionModel()->selection().isEmpty()) {
			if(_model->rowCount() > 0) select_row(0);
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
				emit sig_left_clicked();
			}
			break;

		case Qt::Key_Right:
			if(event->modifiers() & Qt::ControlModifier){
				emit sig_right_clicked();
			}
			break;

		case Qt::Key_Return:
		case Qt::Key_Enter:
			_model->set_current_track(min_row);
			emit sig_double_clicked(min_row);

			break;

		default:
			break;
	}

	if(new_row != -1) {
		goto_row(new_row);
	}
}

void PlaylistView::resizeEvent(QResizeEvent *e) {

	SearchableListView::resizeEvent(e);

	this->set_delegate_max_width(_model->rowCount());
}

void PlaylistView::init_rc_menu() {

	_rc_menu = new PlaylistContextMenu(this);

	connect(_rc_menu, &PlaylistContextMenu::sig_info_clicked, this, &PlaylistView::sig_info_clicked);
	connect(_rc_menu, &PlaylistContextMenu::sig_lyrics_clicked, this, &PlaylistView::sig_lyrics_clicked);
	connect(_rc_menu, &PlaylistContextMenu::sig_edit_clicked, this, &PlaylistView::sig_edit_clicked);
	connect(_rc_menu, &PlaylistContextMenu::sig_remove_clicked, this, &PlaylistView::remove_cur_selected_rows);
	connect(_rc_menu, &PlaylistContextMenu::sig_clear_clicked, this, &PlaylistView::clear);
	connect(_rc_menu, &PlaylistContextMenu::sig_play_next_copy_clicked, this, &PlaylistView::play_next_copy_clicked);
	connect(_rc_menu, &PlaylistContextMenu::sig_play_next_move_clicked, this, &PlaylistView::play_next_move_clicked);
}




void PlaylistView::set_context_menu_actions(int actions) {
	_rc_menu->show_actions(actions);
}

int PlaylistView::get_num_rows() {
	return _model->rowCount();
}

void PlaylistView::set_current_track(int row) {

	QModelIndex new_idx = _model->index(row);
	scrollTo(new_idx, SearchableListView::EnsureVisible);
}


void PlaylistView::clear() {
	_inner_drag_drop = false;
	select_rows(SP::Set<int>());
	_model->clear();
}



void PlaylistView::row_pressed(const QModelIndex& idx) {
	Q_UNUSED(idx)
	if(!idx.isValid()){
		clearSelection();
		return;
	}

	_inner_drag_drop = true;
}

void PlaylistView::row_released(const QModelIndex& idx) {
	Q_UNUSED(idx)
	_inner_drag_drop = false;
}

void PlaylistView::row_double_clicked(const QModelIndex& idx) {
	_inner_drag_drop = false;

	if(idx.isValid()) {
		_model->set_current_track(idx.row());
	}

	emit sig_double_clicked(idx.row());
}


void PlaylistView::selectionChanged ( const QItemSelection& selected, const QItemSelection & deselected ) {

	SearchableListView::selectionChanged(selected, deselected);

	if(selected.indexes().size() > 0){
		scrollTo(selected.indexes()[0], SearchableListView::EnsureVisible);
	}
}

void PlaylistView::scroll_up() {
	QPoint p(5, 5);
	int cur_row = this->indexAt(p).row();
	if(cur_row <= 0) {
		return;
	}

	this->scrollTo(_model->index(cur_row - 1), SearchableListView::EnsureVisible);
}

void PlaylistView::scroll_down() {
	QPoint p(5, this->y() + this->height() - 5);
	int cur_row = this->indexAt(p).row();
	if(cur_row <= 0) {
		return;
	}

	this->scrollTo(_model->index(cur_row - 1), SearchableListView::EnsureVisible);
}

void PlaylistView::remove_cur_selected_rows() {

	int min_row, row_count;

	min_row = get_min_selected();

	_model->remove_rows(get_selections());

	row_count = _model->rowCount();

	clear_selection();

	if(row_count > 0){
		if(min_row >= row_count	){
			select_row(row_count - 1);
		}
		else{
			select_row(min_row);
		}
	}
}


void PlaylistView::play_next_copy_clicked()
{
	SP::Set<int> selected_rows = this->get_selections();
	emit sig_play_next_copy_clicked(selected_rows);
}

void PlaylistView::play_next_move_clicked()
{
	SP::Set<int> selected_rows = this->get_selections();
	emit sig_play_next_move_clicked(selected_rows);
}



void PlaylistView::set_delegate_max_width(int n_items) {

	int row_height = _delegate->get_row_height();
	bool scrollbar_visible = (( n_items * row_height ) >= this->height());

	int max_width = this->width();
	if(scrollbar_visible){
		max_width -= verticalScrollBar()->width();
	}

	_delegate->set_max_width(max_width);
}



// remove the black line under the titles
void  PlaylistView::clear_drag_drop_lines(int row) {

	_delegate->set_drag_index(-1);
	this->update(_model->index(row));
}


int PlaylistView::calc_drag_drop_line(QPoint pos) {

	if(pos.y() < 0) {
		return -1;
	}

	int row = this->indexAt(pos).row();

	if(row <= -1) {
		row = _model->rowCount()-1;
	}

	return row;
}


// the drag comes, if there's data --> accept it
void PlaylistView::dragEnterEvent(QDragEnterEvent* event) {
	event->accept();
}

void PlaylistView::dragMoveEvent(QDragMoveEvent* event) {

	event->accept();

	int row = calc_drag_drop_line(event->pos() );
	bool is_old = _delegate->is_drag_index(row);

	if(!is_old){
		clear_drag_drop_lines(_delegate->get_drag_index());
		_delegate->set_drag_index(row);
		this->update(_model->index(row));
	}
}


// we start the drag action, all lines has to be cleared
void PlaylistView::dragLeaveEvent(QDragLeaveEvent* event) {

	event->accept();
	clear_drag_drop_lines(_delegate->get_drag_index());
}

// called from GUI_Playlist when data has not been dropped
// directly into the view widget. Insert on first row then
void PlaylistView::dropEventFromOutside(QDropEvent* event) {

	event->accept();
	handle_drop(event, true);
}


void PlaylistView::dropEvent(QDropEvent* event) {

	event->accept();
	handle_drop(event, false);
}


void PlaylistView::handle_drop(QDropEvent* event, bool from_outside) {

	MetaDataList v_md;
	QString text;
	SP::Set<int> cur_selected_rows, new_selected_rows;

	// where did i drop?
	int row = _delegate->get_drag_index();
	clear_drag_drop_lines(row);

	if(from_outside) {
		row = -1;
	}

	if(_inner_drag_drop) {
		_inner_drag_drop = false;
		cur_selected_rows = get_selections();

		if( cur_selected_rows.contains(row) ) {
			event->ignore();
			return;
		}


		int i=0;
		for(auto it = cur_selected_rows.begin(); it != cur_selected_rows.end(); it++, i++){
			int row = *it;
			new_selected_rows.insert(row + 1 + i - cur_selected_rows.size());
		}

		_model->move_rows(cur_selected_rows, row + 1);

		this->select_rows( new_selected_rows );

		return;
	}

	const QMimeData* mimedata = event->mimeData();
	const CustomMimeData* custom_mimedata = dynamic_cast<const CustomMimeData*>(mimedata);
	if(!mimedata) {
		return;
	}

	if(mimedata->hasText()) {
		text = mimedata->text();
	}

	// extern
	if( mimedata->hasUrls() && text.compare("tracks", Qt::CaseInsensitive) != 0) {

		QStringList filelist;
		DirectoryReader reader;

		for(const QUrl& url : mimedata->urls()) {

			QString url_str = url.toLocalFile();
			url_str.remove("file://");
			filelist << url_str;
		}

		reader.set_filter(Helper::get_soundfile_extensions());

		v_md = reader.get_md_from_filelist(filelist);

		if(v_md.isEmpty()) {
			return;
		}
	}

	else if(mimedata->hasHtml()) {

	}
	else if(mimedata->hasImage()) {

	}

	else if(custom_mimedata != nullptr){

		if(	custom_mimedata->hasText() &&
			custom_mimedata->hasMetaData())
		{
			v_md = custom_mimedata->getMetaData();
			if(v_md.isEmpty()) {
				return;
			}
		}
	}

	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->insert_tracks(v_md, row+1, plh->get_current_idx());
}


void PlaylistView::fill(PlaylistPtr pl) {

	int n_tracks = pl->get_count();
	set_delegate_max_width(n_tracks);

	for(int i=0; i<n_tracks; i++){

		if(pl->at_const_ref(i).pl_playing) {
			this->scrollTo(_model->index(i), SearchableListView::EnsureVisible);
			return;
		}
	}
}

void PlaylistView::_sl_look_changed(){
	this->update();
	this->repaint();
}

