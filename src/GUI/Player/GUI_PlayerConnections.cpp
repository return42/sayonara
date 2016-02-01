/* GUI_PlayerConnections.cpp */

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

#include "GUI_Player.h"

#include "Components/CoverLookup/CoverLookup.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Components/Engine/EngineHandler.h"

#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"

void GUI_Player::setup_connections() {

	qRegisterMetaType<CoverLocation>("CoverLocation");

	connect(btn_play,	&QPushButton::clicked,	this, &GUI_Player::play_clicked);
	connect(btn_fw,		&QPushButton::clicked,	this, &GUI_Player::next_clicked);
	connect(btn_bw,		&QPushButton::clicked,	this, &GUI_Player::prev_clicked);
	connect(btn_stop,	&QPushButton::clicked,	this, &GUI_Player::stop_clicked);
	connect(btn_mute,	&QPushButton::released,	this, &GUI_Player::mute_button_clicked);
	connect(btn_rec,	&QPushButton::toggled, this, &GUI_Player::rec_clicked);
	connect(albumCover, &QPushButton::clicked, this, &GUI_Player::cover_clicked);

	connect(_play_manager, &PlayManager::sig_playstate_changed, this, &GUI_Player::playstate_changed);
	connect(_play_manager, &PlayManager::sig_track_changed, this, &GUI_Player::track_changed);
	connect(_play_manager, &PlayManager::sig_position_changed_ms, this,	&GUI_Player::set_cur_pos_ms);
	connect(_play_manager, &PlayManager::sig_buffer, this, &GUI_Player::buffering);
	connect(_play_manager, &PlayManager::sig_volume_changed, this, &GUI_Player::volume_changed);
	connect(_play_manager, &PlayManager::sig_mute_changed, this, &GUI_Player::mute_changed);

	// engine
	connect(_engine, &EngineHandler::sig_md_changed,	this, &GUI_Player::md_changed);
	connect(_engine, &EngineHandler::sig_dur_changed, this, &GUI_Player::dur_changed);
	connect(_engine, &EngineHandler::sig_br_changed,	this, &GUI_Player::br_changed);

	// file
	connect(action_OpenFile, &QAction::triggered, this, &GUI_Player::open_files_clicked);
	connect(action_OpenFolder, &QAction::triggered, this, &GUI_Player::open_dir_clicked);
	connect(action_Close, &QAction::triggered, this, &GUI_Player::really_close);

#ifdef WITH_MTP
	connect(action_devices, &QAction::triggered, _mtp, &GUI_MTP::show);
#endif


	// view
	connect(action_viewLibrary, &QAction::toggled, this, &GUI_Player::show_library);
	connect(action_Dark, &QAction::toggled, this, &GUI_Player::skin_toggled);
	connect(action_Fullscreen, &QAction::toggled, this, &GUI_Player::show_fullscreen_toggled);

	connect(splitter, &QSplitter::splitterMoved, this, &GUI_Player::main_splitter_moved);


	// preferencesF
	connect(action_setLibPath, &QAction::triggered, this, &GUI_Player::set_library_path_clicked);
	connect(action_min2tray, &QAction::toggled,	this, &GUI_Player::min2tray_toggled);
	connect(action_only_one_instance, &QAction::toggled, this, &GUI_Player::only_one_instance_toggled);
	connect(action_livesearch, &QAction::triggered, this, &GUI_Player::live_search_toggled);
	connect(action_notifyNewVersion, &QAction::triggered, this,	&GUI_Player::notify_new_version_toggled);


	// about
	connect(action_about, &QAction::triggered, this, &GUI_Player::about);
	connect(action_help, &QAction::triggered, this, &GUI_Player::help);
	connect(sli_volume, &SearchSlider::sig_slider_moved, this, &GUI_Player::volume_slider_moved);
	connect(sli_progress, &SearchSlider::sig_slider_moved, this, &GUI_Player::seek);


	MetaDataChangeNotifier* md_change_notifier = MetaDataChangeNotifier::getInstance();
	connect(md_change_notifier, &MetaDataChangeNotifier::sig_metadata_changed, this, &GUI_Player::id3_tags_changed);

	// cover lookup
	connect(_cov_lookup, &CoverLookup::sig_cover_found, this, &GUI_Player::set_cover_image);
	connect(_ui_alternative_covers, &GUI_AlternativeCovers::sig_cover_changed, this, &GUI_Player::set_cover_image);

	//connect(_fsw, &QFileSystemWatcher::directoryChanged, this, &GUI_Player::read_filelist);

    QList<QKeySequence> lst;
	lst << QKeySequence(Qt::Key_Space);

	QAction* play_pause_action = create_actions(lst);
	connect(play_pause_action, &QAction::triggered, btn_play, &QPushButton::click);

	sp_log(Log::Debug) << "connections done";
}

