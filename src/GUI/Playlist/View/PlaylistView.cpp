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

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/SayonaraWidget/SayonaraLoadingBar.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Parser/StreamParser.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Components/Playlist/PlaylistHandler.h"

#include <QShortcut>

PlaylistView::PlaylistView(PlaylistPtr pl, QWidget* parent) :
	SearchableListView(parent)
{
	_model = new PlaylistItemModel(pl, this);
	_delegate = new PlaylistItemDelegate(this);

	_drag = nullptr;

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

	REGISTER_LISTENER(Set::PL_EntryLook, _sl_look_changed);
}

PlaylistView::~PlaylistView() {

	if(_rc_menu){
		delete _rc_menu;
	}

	delete _model;
	delete _delegate;
}

void PlaylistView::init_rc_menu() {

	_rc_menu = new LibraryContextMenu(this);

	connect(_rc_menu, &LibraryContextMenu::sig_info_clicked, this, &PlaylistView::sig_info_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_lyrics_clicked, this, &PlaylistView::sig_lyrics_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_edit_clicked, this, &PlaylistView::sig_edit_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_remove_clicked, this, &PlaylistView::remove_cur_selected_rows);
	connect(_rc_menu, &LibraryContextMenu::sig_clear_clicked, this, &PlaylistView::clear);
}

void PlaylistView::set_context_menu_actions(int actions) {
	_rc_menu->show_actions(actions);
}


int PlaylistView::get_num_rows() {
	return _model->rowCount();
}

void PlaylistView::goto_row(int row) {

	row = std::max(row, 0);
	row = std::min(row, _model->rowCount() - 1);

	if(row == -1){
		return;
	}

	QModelIndex idx = _model->index(row, 0);

	this->scrollTo(idx, SearchableListView::EnsureVisible);
}


void PlaylistView::clear() {

	clear_selection();
	_model->clear();
}


void PlaylistView::selectionChanged ( const QItemSelection& selected, const QItemSelection & deselected ) {

	SearchableListView::selectionChanged(selected, deselected);
	if(selected.indexes().isEmpty()){
		return;
	}

	goto_row(selected.indexes().first().row());
}

void PlaylistView::scroll_up() {
	QPoint p(5, 5);

	QModelIndex idx = this->indexAt(p);

	goto_row(idx.row() - 1);
}

void PlaylistView::scroll_down() {
	QPoint p(5, this->y() + this->height() - 5);

	QModelIndex idx = this->indexAt(p);

	goto_row(idx.row() + 1);
}

void PlaylistView::remove_cur_selected_rows() {

	int min_row = get_min_selected();

	_model->remove_rows(get_selections());
	clear_selection();

	int row_count = _model->rowCount();

	if(row_count > 0){
		if(min_row >= row_count	){
			select_row(row_count - 1);
		}
		else{
			select_row(min_row);
		}
	}
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

void PlaylistView::handle_drop(QDropEvent* event) {

	const QMimeData* mimedata = event->mimeData();
	PlaylistHandler* plh = PlaylistHandler::getInstance();

	MetaDataList v_md;
	QStringList www_playlists;
	QString src;

	int row = _delegate->get_drag_index();
	clear_drag_drop_lines(row);

	if(!mimedata){
		return;
	}

	src = mimedata->objectName();
	bool inner_drag_drop = (src.compare("inner") == 0);
	if(inner_drag_drop) {
		bool copy = (event->keyboardModifiers() & Qt::ControlModifier);
		handle_inner_drag_drop(row, copy);
		return;
	}

	const CustomMimeData* custom_mimedata = dynamic_cast<const CustomMimeData*>(mimedata);

	if(custom_mimedata){
		if(	custom_mimedata->hasText() &&
			custom_mimedata->hasMetaData())
		{
			v_md = custom_mimedata->getMetaData();
		}
	}

	else if( mimedata->hasUrls()) {


		DirectoryReader reader;
		reader.set_filter(Helper::get_soundfile_extensions());

		for(const QUrl& url : mimedata->urls()) {
			if(url.isLocalFile()){
				QStringList file_list;
				MetaDataList v_md_tmp;
				file_list << url.toLocalFile();
				v_md_tmp = reader.get_md_from_filelist(file_list);
				v_md << v_md_tmp;
			}

			else if(Helper::File::is_playlistfile(url.toString())){
				www_playlists << url.toString();
			}
		}
	}

	if(!v_md.isEmpty()) {
		plh->insert_tracks(v_md, row+1, plh->get_current_idx());
	}

	if(!www_playlists.isEmpty()){

		this->setEnabled(false);
		if(!_progress){
			_progress = new SayonaraLoadingBar(this);
		}

		_progress->show();

		_async_drop_index = row;
		StreamParser* stream_parser = new StreamParser();
		if(mimedata->hasText()){
			stream_parser->set_cover_url(mimedata->text());
		}

		connect(stream_parser, &StreamParser::sig_finished, this, &PlaylistView::handle_async_drop);
		stream_parser->parse_streams(www_playlists);
	}
}

void PlaylistView::handle_async_drop(bool success){
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	StreamParser* stream_parser = dynamic_cast<StreamParser*>(sender());
	this->setEnabled(true);

	_progress->hide();

	if(success){
		MetaDataList v_md = stream_parser->get_metadata();
		plh->insert_tracks(v_md, _async_drop_index+1, plh->get_current_idx());
	}

	stream_parser->deleteLater();
}

void PlaylistView::handle_inner_drag_drop(int row, bool copy)
{
	SP::Set<int> cur_selected_rows, new_selected_rows;
	int n_lines_before_tgt = 0;

	cur_selected_rows = get_selections();

	if( cur_selected_rows.contains(row) ) {
		return;
	}

	if(copy){
		_model->copy_rows(cur_selected_rows, row + 1);
		emit sig_time_changed();
	}

	else {
		_model->move_rows(cur_selected_rows, row + 1);
		n_lines_before_tgt = std::count_if(cur_selected_rows.begin(), cur_selected_rows.end(), [&row](int sel){
			return sel < row;
		});
	}

	for(int i=row; i<row + (int) cur_selected_rows.size(); i++){
		new_selected_rows.insert(i - n_lines_before_tgt + 1);
	}

	this->select_rows( new_selected_rows );
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

