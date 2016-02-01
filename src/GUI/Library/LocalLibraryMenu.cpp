/* LocalLibraryMenu.cpp */

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



#include "LocalLibraryMenu.h"
#include "Helper/Helper.h"



LocalLibraryMenu::LocalLibraryMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{
	_timer = new QTimer(this);

	_reload_library_action = new QAction(Helper::get_icon("undo"), tr("Reload library"), this);
	_import_file_action = new QAction(Helper::get_icon("open"), tr("Import files"), this);
	_import_folder_action = new QAction(Helper::get_icon("open"), tr("Import directory"), this);
	_info_action = new QAction(Helper::get_icon("info"), tr("Info"), this);

	connect(_timer, &QTimer::timeout, this, &LocalLibraryMenu::timed_out);
	connect(_reload_library_action, &QAction::triggered, this, &LocalLibraryMenu::sig_reload_library);
	connect(_import_file_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_file);
	connect(_import_folder_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_folder);
	connect(_info_action, &QAction::triggered, this, &LocalLibraryMenu::sig_info);

	_actions << _info_action << this->addSeparator() << _import_file_action << _import_folder_action << _reload_library_action;

	this->addActions(_actions);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}


LocalLibraryMenu::~LocalLibraryMenu()
{

}

void LocalLibraryMenu::showEvent(QShowEvent* e)
{
	for(QAction* action : _actions){
		action->setEnabled(false);
	}

	QMenu::showEvent(e);

	_timer->setInterval(250);
	_timer->start();
}

void LocalLibraryMenu::timed_out(){
	for(QAction* action : _actions){
		action->setEnabled(true);
	}

}

void LocalLibraryMenu::language_changed(){
	_reload_library_action->setText(tr("Reload library"));
	_import_file_action->setText(tr("Import files"));
	_import_folder_action->setText(tr("Import directory"));
	_info_action->setText(tr("Info"));
}

