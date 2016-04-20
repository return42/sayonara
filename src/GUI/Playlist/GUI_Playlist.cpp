/* GUI_Playlist.cpp */

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
 * GUI_Playlist.cpp
 *
 *  Created on: Apr 6, 2011
 *      Author: luke
 */

#include "GUI_Playlist.h"
#include "GUI/InfoDialog/GUI_InfoDialog.h"
#include "View/PlaylistView.h"
#include "Helper/Helper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "TabWidget/PlaylistTabWidget.h"

#include <QKeyEvent>
#include <QFileDialog>
#include <QScrollBar>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QTimer>

GUI_Playlist::GUI_Playlist(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::Playlist_Window()
{
	setupUi(this);

	_playlist = PlaylistHandler::getInstance();
	_play_manager = PlayManager::getInstance();
	_info_dialog = new GUI_InfoDialog(this);

	bottom_bar->check_dynamic_play_button();

    setAcceptDrops(true);

	connect(_playlist,	&PlaylistHandler::sig_cur_track_idx_changed,
			this,		&GUI_Playlist::playlist_track_changed);
	connect(_playlist,	&PlaylistHandler::sig_playlist_created,
			this,		&GUI_Playlist::playlist_fill);
	connect(_playlist,	&PlaylistHandler::sig_playlist_name_changed,
			this,		&GUI_Playlist::playlist_name_changed);
	connect(_playlist,	&PlaylistHandler::sig_new_playlist_added,
			this,		&GUI_Playlist::playlist_added);
	connect(_playlist,  &PlaylistHandler::sig_playlist_idx_changed,
			this,		&GUI_Playlist::playlist_idx_changed);

	connect(_play_manager, &PlayManager::sig_playlist_finished,	this, &GUI_Playlist::playlist_finished);
	connect(_play_manager, &PlayManager::sig_playstate_changed,	this, &GUI_Playlist::playstate_changed);

	connect(tw_playlists, &PlaylistTabWidget::sig_add_tab_clicked, this, &GUI_Playlist::add_playlist_button_pressed);
	connect(tw_playlists, &PlaylistTabWidget::tabCloseRequested, this, &GUI_Playlist::tab_close_playlist_clicked);
	connect(tw_playlists, &PlaylistTabWidget::currentChanged, _playlist, &PlaylistHandler::set_current_idx);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_delete, this, &GUI_Playlist::tab_delete_playlist_clicked);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_save, this, &GUI_Playlist::tab_save_playlist_clicked);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_save_as, this, &GUI_Playlist::tab_save_playlist_as_clicked);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_rename, this, &GUI_Playlist::tab_rename_clicked);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_clear, this, &GUI_Playlist::clear_button_pressed);
	connect(tw_playlists, &PlaylistTabWidget::sig_tab_reset, _playlist, &PlaylistHandler::reset_playlist);
	connect(tw_playlists, &PlaylistTabWidget::sig_metadata_dropped, this, &GUI_Playlist::tab_metadata_dropped);

	REGISTER_LISTENER(Set::Lib_Path, _sl_library_path_changed);
	REGISTER_LISTENER(Set::PL_ShowNumbers, _sl_show_numbers_changed);

	init_shortcuts();
	load_old_playlists();

	for(int i=0; i<tw_playlists->count(); i++){
		PlaylistView* plv = get_view_by_idx(i);
		if(!plv){
			continue;
		}

		plv->setObjectName("playlist_view" + QString::number(i));
	}

}


GUI_Playlist::~GUI_Playlist() {

	while(tw_playlists->count() > 0){
		tw_playlists->removeTab(0);
	}

	while(tw_playlists->count() > 1){

		QWidget* widget = tw_playlists->widget(0);
		tw_playlists->removeTab(0);

		if(widget){
			delete widget;
		}
	}

	delete _info_dialog;
}

void GUI_Playlist::changeEvent(QEvent* e) {
    e->accept();
}

void GUI_Playlist::resizeEvent(QResizeEvent* e) {

	PlaylistView* cur_view = get_current_view();

    e->accept();

	if(cur_view){
		cur_view->update();
		cur_view->reset();
	}
}

void GUI_Playlist::language_changed() {

	retranslateUi(this);
	set_total_time_label();

	bottom_bar->check_dynamic_play_button();
}

void GUI_Playlist::skin_changed()
{
	check_tab_icon();
}


void GUI_Playlist::playlist_track_changed(int row, int playlist_idx) {

	check_tab_icon();

	int n_rows;
	PlaylistView* plv = get_view_by_idx(playlist_idx);

	if( !plv ) {
		return;
	}

	plv->set_current_track(row);
	n_rows = plv->get_num_rows();

	if(n_rows > 0){
		IconLoader* icon_loader = IconLoader::getInstance();
		QIcon icon_play = icon_loader->get_icon("media-playback-start", "play_bordered");

		tw_playlists->setTabIcon(playlist_idx, icon_play);
	}
}


void GUI_Playlist::playlist_fill(PlaylistPtr pl) {

	int pl_idx = pl->get_idx();
	PlaylistView* plv = get_view_by_idx(pl_idx);

	if( !plv ) {
		return;
	}

	plv->setObjectName("playlist_view" + QString::number(pl_idx));
	plv->fill( pl );

	_total_time[pl_idx] = pl->get_running_time();

	set_total_time_label();
	check_playlist_name(pl);
}


void GUI_Playlist::clear_button_pressed(int pl_idx) {

	_playlist->clear_playlist(pl_idx);

	if(between(pl_idx, 0, _total_time.size())){
		_total_time[pl_idx] = 0;
	}

	set_total_time_label();
}


void GUI_Playlist::add_playlist_button_pressed(){

	QString name = _playlist->request_new_playlist_name();
	_playlist->create_empty_playlist(name);
}

void GUI_Playlist::tab_metadata_dropped(int pl_idx, const MetaDataList& v_md)
{

	if(pl_idx < 0){
		return;
	}

	sp_log(Log::Debug) << "Was drag from playlist? " << tw_playlists->was_drag_from_playlist();
	if(tw_playlists->was_drag_from_playlist()){

		int origin_tab = tw_playlists->get_drag_origin_tab();
		sp_log(Log::Debug) << "origin tab " <<origin_tab;
		PlaylistView* plv = get_view_by_idx(origin_tab);

		if(plv){
			plv->remove_cur_selected_rows();
		}
	}

	QString name = _playlist->request_new_playlist_name();

	if(pl_idx == tw_playlists->count() - 1){
		_playlist->create_playlist(v_md, name);
	}

	else{
		_playlist->append_tracks(v_md, pl_idx);
	}
}


void GUI_Playlist::double_clicked(int row) {

	int cur_idx = tw_playlists->currentIndex();
	_playlist->change_track(row, cur_idx);
}

void GUI_Playlist::fill_info_dialog(){

	int cur_idx = _playlist->get_current_idx();

	PlaylistConstPtr pl = _playlist->get_playlist_at(cur_idx);
	PlaylistView* cur_view = get_view_by_idx(cur_idx);

	if(pl == nullptr || cur_view == nullptr){
		return;
	}

	SP::Set<int> selections = cur_view->get_selections();
	MetaDataList v_md = pl->get_playlist().extract_tracks(selections);

	_info_dialog->set_metadata(v_md, GUI_InfoDialog::Mode::Tracks );
}

void GUI_Playlist::init_shortcuts()
{

}


void GUI_Playlist::menu_info_clicked() {

    if(!_info_dialog) return;

	fill_info_dialog();

	_info_dialog->show(GUI_InfoDialog::TabInfo);
}


void GUI_Playlist::menu_edit_clicked() {
    if(!_info_dialog) return;

	fill_info_dialog();

	_info_dialog->show(GUI_InfoDialog::TabEdit);
}

void GUI_Playlist::menu_lyrics_clicked()
{
	if(!_info_dialog) return;

	fill_info_dialog();

	_info_dialog->show(GUI_InfoDialog::TabLyrics);
}


void GUI_Playlist::dragLeaveEvent(QDragLeaveEvent* event) {
    event->accept();
}


void GUI_Playlist::dragEnterEvent(QDragEnterEvent* event) {
    event->accept();
}


void GUI_Playlist::dragMoveEvent(QDragMoveEvent* event) {

	PlaylistView* cur_view = get_current_view();

	if(!cur_view){
		return;
	}

	int y = event->pos().y();

	if( y < cur_view->y()){
		cur_view->scroll_up();
	}

	else if(y > cur_view->y() + cur_view->height()){
		cur_view->scroll_down();
	}
}


void GUI_Playlist::dropEvent(QDropEvent* event) {
	PlaylistView* cur_view = get_current_view();
	if(cur_view){
		cur_view->dropEventFromOutside(event);
	}
}

void GUI_Playlist::set_total_time_label() {

	PlaylistView* cur_view;

	int n_rows = 0;
	QString text;
	QString time_str;
	QString playlist_string;
	int idx;
	qint64 dur_ms;

	idx = tw_playlists->currentIndex();
	if(!between(idx, 0, _total_time.size())){
		dur_ms = 0;
	}

	else{
		dur_ms = _total_time[idx];
	}

	if(dur_ms > 0){
		time_str = Helper::cvt_ms_to_string(dur_ms, true, false);
	}

	cur_view = get_current_view();
	if(cur_view){
		n_rows = cur_view->get_num_rows();
	}

	playlist_string = text + QString::number(n_rows);


	if(n_rows == 0){
		playlist_string = tr("Playlist empty");
	}

	else if(n_rows == 1)	{
		playlist_string += " " + tr("Track");
	}

	else {
		playlist_string += " " + tr("Tracks");
	}

	if( dur_ms > 0 ){
		playlist_string += " - " + time_str;
	}

	lab_totalTime->setText(playlist_string);
	lab_totalTime->setContentsMargins(0, 2, 0, 2);
}



void GUI_Playlist::_sl_show_numbers_changed(){

	PlaylistView* cur_view = get_current_view();

	parentWidget()->setFocus();

	if( cur_view ){
	   cur_view->reset();
	}
}


void GUI_Playlist::_sl_library_path_changed() {

	bottom_bar->check_dynamic_play_button();
}

void GUI_Playlist::load_old_playlists()
{
	_playlist->load_old_playlists();
}
