/*
 * GUI_PlayerMenubar.cpp
 *
 *  Created on: 10.10.2012
 *      Author: luke
 */

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

#include "Components/CoverLookup/CoverLookupAll.h"
#include "Components/Library/LibraryImporter.h"
#include "Components/Library/LocalLibrary.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Helper/WebAccess/AsyncWebAccess.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

/** FILE **/
void GUI_Player::open_files_clicked() {

	QStringList filetypes;

	filetypes << Helper::get_soundfile_extensions();
	filetypes << Helper::get_playlistfile_extensions();

	QString filetypes_str = QString(tr("Media files") + " (");
	for(const QString& filetype : filetypes) {
		filetypes_str += filetype;
		if(filetype != filetypes.last()) {
			filetypes_str += " ";
		}
	}
	filetypes_str += ")";

	QStringList list =
			QFileDialog::getOpenFileNames(
					this,
					tr("Open Media files"),
					QDir::homePath(),
					filetypes_str);

	if (list.size() > 0){
		PlaylistHandler* plh = PlaylistHandler::getInstance();
		plh->create_playlist(list);
	}
}

void GUI_Player::open_dir_clicked() {

	QString dir = QFileDialog::getExistingDirectory(this,
			tr("Open Directory"),
			QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (dir != ""){
		PlaylistHandler* plh = PlaylistHandler::getInstance();
		plh->create_playlist(dir);
	}
}

bool GUI_Player::check_library_path(){

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.size() == 0 || !QFile::exists(lib_path)) {

		GlobalMessage::Answer ret = Message::info(tr("Please select library path first"));
		if(ret == GlobalMessage::Answer::Cancel) {
			return false;
		}

		set_library_path_clicked();

		return false;
	}

	return true;
}

/** FILE END **/


/** VIEW **/

void GUI_Player::show_library(bool b) {

	QSize player_size;
	int library_width;

	player_size = this->size();

	_settings->set(Set::Lib_Show, b);

	library_width = library_widget->width();
	library_widget->setVisible(b);

	if(!b){
		if(_settings->get(Set::Lib_OldWidth) == 0){
			_settings->set(Set::Lib_OldWidth, library_width);
		}

		player_size.setWidth( player_size.width() - library_width);
	}

	else{
		library_width = _settings->get(Set::Lib_OldWidth);
		_settings->set(Set::Lib_OldWidth, 0);

		if(library_width < 100){
			library_width = 400;
		}


		player_size.setWidth( player_size.width() + library_width);
	}

	check_library_menu_action();

	this->resize(player_size);
}

void GUI_Player::_sl_fullscreen_toggled(){
	show_fullscreen_toggled(_settings->get(Set::Player_Fullscreen));
}

void GUI_Player::show_fullscreen_toggled(bool b) {
	// may happend because of F11 too
	action_Fullscreen->setChecked(b);
	if(b){
		showFullScreen();
	}

	else {
		showNormal();
	}

	_settings->set(Set::Player_Fullscreen, b);
}


/** VIEW END **/



/** PREFERENCES **/

void GUI_Player::set_library_path_clicked() {

	QString start_dir = QDir::homePath();
	QString new_dir;
	QString old_dir = Helper::File::get_absolute_filename(_settings->get(Set::Lib_Path));

	if(!_local_library){
		_local_library = LocalLibrary::getInstance();
	}

	if (old_dir.size() > 0 && QFile::exists(old_dir)) {
		start_dir = old_dir;
    }

	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
			old_dir, QFileDialog::ShowDirsOnly);

    if (dir.size() > 0 && (old_dir.compare(dir) != 0)) {

		_settings->set(Set::Lib_Path, dir);
		new_dir = dir;

		GlobalMessage::Answer answer = Message::question_yn(tr("Do you want to reload the Library?"), "Library");

		if(answer == GlobalMessage::Answer::Yes){
			bool clear_first = (old_dir != new_dir);

			_local_library->psl_reload_library(clear_first, Tagging::Quality::Standard);
		}
	}
}


// prvt slot
void GUI_Player::show_notification_toggled(bool active) {
	_settings->set(Set::Notification_Show, active);
}

// prvt slot
void GUI_Player::min2tray_toggled(bool b) {
	_settings->set(Set::Player_Min2Tray, b);
}

void GUI_Player::only_one_instance_toggled(bool b) {
	_settings->set(Set::Player_OneInstance, b);
}


void GUI_Player::live_search_toggled(bool b) {
	_settings->set(Set::Lib_LiveSearch, b);
}

/** PREFERENCES END **/

void GUI_Player::help() {

	QString link = Helper::create_link("http://sayonara-player.com/forum");
	Message::info(tr("Please visit the forum at") + "<br />" + link);
}

// private slot
void GUI_Player::about() {

	QString first_translators;
	QString last_translator;
	QString translator_str = "";
	QString version = _settings->get(Set::Player_Version);

	QString link = Helper::create_link("http://sayonara-player.com");

	if(!_about_box){
		_about_box = new QMessageBox(this);
		_about_box->setParent(this);
		_about_box->setIconPixmap(Helper::get_pixmap("logo.png", QSize(150, 150), true));
		_about_box->setWindowFlags(Qt::Dialog);
		_about_box->setModal(true);
		_about_box->setStandardButtons(QMessageBox::Ok);
		_about_box->setWindowTitle(tr("About Sayonara"));
		_about_box->setText("<b><font size=\"+2\">Sayonara Player " + version + "</font></b>");

		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->run("http://sayonara-player.com/translators");
		connect(awa,  &AsyncWebAccess::sig_finished, this, &GUI_Player::awa_translators_finished);

	}

	if(_translators.size() > 2) {

		for (int i=0; i<_translators.size() - 1; i++) {

			first_translators += "<b>" + _translators[i] + "</b>";
			if(i < _translators.size() - 2) first_translators += ", ";
        }

		last_translator = QString("<b>") + _translators[_translators.size() - 1] + "</b>";
        translator_str = QString("<br /><br /><br />") +
                tr("Special thanks to %1 and %2 for translating")
                .arg(first_translators)
                .arg(last_translator);
    }

	_about_box->setInformativeText( QString("") +
				tr("Written by Lucio Carreras") + "<br /><br />" +
                tr("License") + ": GPLv3<br /><br />" +
				"Copyright 2011-" + QString::number(QDateTime::currentDateTime().date().year()) +
				"<br /><br />" + link + translator_str
	);


	if(!_about_box->isVisible()){
		_about_box->exec();
	}
}


