/* LocalLibraryMenu.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
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
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"
#include "Database/DatabaseHandler.h"
#include "Database/LibraryDatabase.h"

LocalLibraryMenu::LocalLibraryMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{
	_icon_loader = IconLoader::getInstance();

	_reload_library_action = new QAction(GUI::get_icon("undo"), QString(), this);
	_import_file_action = new QAction(GUI::get_icon("open"), QString(), this);
	_import_folder_action = new QAction(GUI::get_icon("open"), QString(), this);
	_info_action = new QAction(GUI::get_icon("info"), QString(), this);
	_library_path_action = new QAction(GUI::get_icon("folder"), QString(), this);

	_realtime_search_action = new QAction(QIcon(), tr("Live search"), this);
	_realtime_search_action->setCheckable(true);
	_realtime_search_action->setChecked(_settings->get(Set::Lib_LiveSearch));
	_auto_update = new QAction(QIcon(), tr("Auto update"), this);
	_auto_update->setCheckable(true);
	_auto_update->setChecked(_settings->get(Set::Lib_AutoUpdate));

	_show_album_artists_action = new QAction(QIcon(), QString(), this);
	_show_album_artists_action->setCheckable(true);
	_show_album_artists_action->setChecked(_settings->get(Set::Lib_ShowAlbumArtists));

	_show_album_cover_view = new QAction(QIcon(), QString(), this);
	_show_album_cover_view->setCheckable(true);
	_show_album_cover_view->setChecked(_settings->get(Set::Lib_ShowAlbumCovers));

	connect(_reload_library_action, &QAction::triggered, this, &LocalLibraryMenu::sig_reload_library);
	connect(_import_file_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_file);
	connect(_import_folder_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_folder);
	connect(_info_action, &QAction::triggered, this, &LocalLibraryMenu::sig_info);
	connect(_library_path_action, &QAction::triggered, this, &LocalLibraryMenu::sig_libpath_clicked);
	connect(_realtime_search_action, &QAction::triggered, this, &LocalLibraryMenu::realtime_search_changed);
	connect(_auto_update, &QAction::triggered, this, &LocalLibraryMenu::auto_update_changed);
	connect(_show_album_artists_action, &QAction::triggered, this, &LocalLibraryMenu::show_album_artists_changed);
	connect(_show_album_cover_view, &QAction::triggered, this, &LocalLibraryMenu::show_album_cover_view_changed);

	_actions <<_library_path_action <<
				this->addSeparator() <<

				_info_action <<
				this->addSeparator() <<
				_import_file_action <<
				_import_folder_action <<
				_reload_library_action <<
				this->addSeparator() <<
				_show_album_cover_view <<
				_realtime_search_action <<
				_auto_update <<
				_show_album_artists_action;

	this->addActions(_actions);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

LocalLibraryMenu::~LocalLibraryMenu() {}


void LocalLibraryMenu::language_changed()
{
	_reload_library_action->setText(Lang::get(Lang::ReloadLibrary));
	_import_file_action->setText(Lang::get(Lang::ImportFiles));
	_import_folder_action->setText(Lang::get(Lang::ImportDir));
	_info_action->setText(Lang::get(Lang::Info));
	_library_path_action->setText(Lang::get(Lang::LibraryPath));
	_realtime_search_action->setText(tr("Live search"));
	_auto_update->setText(tr("Auto update"));
	_show_album_artists_action->setText(Lang::get(Lang::ShowAlbumArtists));
	_show_album_cover_view->setText(tr("Show album covers"));
}

void LocalLibraryMenu::skin_changed()
{
	_reload_library_action->setIcon(_icon_loader->get_icon("view-refresh", "undo"));
	_import_file_action->setIcon(_icon_loader->get_icon("document-open", "open"));
	_import_folder_action->setIcon(_icon_loader->get_icon("document-open", "open"));
	_info_action->setIcon(_icon_loader->get_icon("dialog-information", "info"));
	_library_path_action->setIcon(_icon_loader->get_icon("folder", "folder"));
}

void LocalLibraryMenu::realtime_search_changed()
{
	_settings->set(Set::Lib_LiveSearch, _realtime_search_action->isChecked());
}

void LocalLibraryMenu::auto_update_changed()
{
	_settings->set(Set::Lib_AutoUpdate, _auto_update->isChecked());
}

void LocalLibraryMenu::show_album_cover_view_changed()
{
	_settings->set(Set::Lib_ShowAlbumCovers, _show_album_cover_view->isChecked());
}


void LocalLibraryMenu::show_album_artists_changed()
{
	bool show_album_artist = _show_album_artists_action->isChecked();
	_settings->set(Set::Lib_ShowAlbumArtists, show_album_artist);

	if(show_album_artist){
		DB::getInstance()->get_std()->change_artistid_field(LibraryDatabase::ArtistIDField::AlbumArtistID);
	}

	else{
		DB::getInstance()->get_std()->change_artistid_field(LibraryDatabase::ArtistIDField::ArtistID);
	}

	emit sig_show_album_artists_changed();
}

