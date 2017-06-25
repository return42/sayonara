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
#include "GUI/Helper/Library/GUI_EditLibrary.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"


struct LocalLibraryMenu::Private
{
	QString name;
	QString path;

	QAction* reload_library_action=nullptr;
	QAction* import_file_action=nullptr;
	QAction* import_folder_action=nullptr;
	QAction* info_action=nullptr;
	QAction* edit_action=nullptr;
	QAction* realtime_search_action=nullptr;
	QAction* auto_update=nullptr;
	QAction* show_album_artists_action=nullptr;
	QAction* show_album_cover_view=nullptr;

	IconLoader* icon_loader=nullptr;

	QList<QAction*> actions;

	Private(const QString& name, const QString& path) :
		name(name),
		path(path)
	{}
};

LocalLibraryMenu::LocalLibraryMenu(const QString& name, const QString& path, QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{
	_m = Pimpl::make<Private>(name, path);

	_m->icon_loader = IconLoader::getInstance();

	_m->reload_library_action = new QAction(QIcon(), QString(), this);
	_m->import_file_action = new QAction(QIcon(), QString(), this);
	_m->import_folder_action = new QAction(QIcon(), QString(), this);
	_m->info_action = new QAction(QIcon(), QString(), this);
	_m->edit_action = new QAction(QIcon(), QString(), this);

	_m->realtime_search_action = new QAction(QIcon(), tr("Live search"), this);
	_m->realtime_search_action->setCheckable(true);
	_m->realtime_search_action->setChecked(_settings->get(Set::Lib_LiveSearch));
	_m->auto_update = new QAction(QIcon(), tr("Auto update"), this);
	_m->auto_update->setCheckable(true);
	_m->auto_update->setChecked(_settings->get(Set::Lib_AutoUpdate));

	_m->show_album_artists_action = new QAction(QIcon(), QString(), this);
	_m->show_album_artists_action->setCheckable(true);
	_m->show_album_artists_action->setChecked(_settings->get(Set::Lib_ShowAlbumArtists));

	_m->show_album_cover_view = new QAction(QIcon(), QString(), this);
	_m->show_album_cover_view->setCheckable(true);
	_m->show_album_cover_view->setChecked(_settings->get(Set::Lib_ShowAlbumCovers));

	connect(_m->reload_library_action, &QAction::triggered, this, &LocalLibraryMenu::sig_reload_library);
	connect(_m->import_file_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_file);
	connect(_m->import_folder_action, &QAction::triggered, this, &LocalLibraryMenu::sig_import_folder);
	connect(_m->info_action, &QAction::triggered, this, &LocalLibraryMenu::sig_info);
	connect(_m->edit_action, &QAction::triggered, this, &LocalLibraryMenu::edit_clicked);
	connect(_m->realtime_search_action, &QAction::triggered, this, &LocalLibraryMenu::realtime_search_changed);
	connect(_m->auto_update, &QAction::triggered, this, &LocalLibraryMenu::auto_update_changed);
	connect(_m->show_album_artists_action, &QAction::triggered, this, &LocalLibraryMenu::show_album_artists_changed);
	connect(_m->show_album_cover_view, &QAction::triggered, this, &LocalLibraryMenu::show_album_cover_view_changed);

	_m->actions << _m->edit_action <<
				this->addSeparator() <<

				_m->info_action <<
				this->addSeparator() <<
				_m->import_file_action <<
				_m->import_folder_action <<
				_m->reload_library_action <<
				this->addSeparator() <<
				_m->show_album_cover_view <<
				_m->realtime_search_action <<
				_m->auto_update <<
				_m->show_album_artists_action;

	this->addActions(_m->actions);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

LocalLibraryMenu::~LocalLibraryMenu() {}

void LocalLibraryMenu::refresh_name(const QString& name)
{
	_m->name = name;
}

void LocalLibraryMenu::refresh_path(const QString& path)
{
	_m->path = path;
}


void LocalLibraryMenu::language_changed()
{
	_m->reload_library_action->setText(Lang::get(Lang::ReloadLibrary));
	_m->import_file_action->setText(Lang::get(Lang::ImportFiles));
	_m->import_folder_action->setText(Lang::get(Lang::ImportDir));
	_m->info_action->setText(Lang::get(Lang::Info));
	_m->edit_action->setText(Lang::get(Lang::Edit));
	_m->realtime_search_action->setText(tr("Live search"));
	_m->auto_update->setText(tr("Auto update"));
	_m->show_album_artists_action->setText(Lang::get(Lang::ShowAlbumArtists));
	_m->show_album_cover_view->setText(tr("Show album covers"));
}

void LocalLibraryMenu::skin_changed()
{
	_m->reload_library_action->setIcon(_m->icon_loader->get_icon("view-refresh", "undo"));
	_m->import_file_action->setIcon(_m->icon_loader->get_icon("document-open", "open"));
	_m->import_folder_action->setIcon(_m->icon_loader->get_icon("document-open", "open"));
	_m->info_action->setIcon(_m->icon_loader->get_icon("dialog-information", "info"));
	_m->edit_action->setIcon(_m->icon_loader->get_icon("accessories-text-editor", "edit"));
}

void LocalLibraryMenu::realtime_search_changed()
{
	_settings->set(Set::Lib_LiveSearch, _m->realtime_search_action->isChecked());
}

void LocalLibraryMenu::auto_update_changed()
{
	_settings->set(Set::Lib_AutoUpdate, _m->auto_update->isChecked());
}

void LocalLibraryMenu::edit_clicked()
{
	GUI_EditLibrary* edit_dialog = new GUI_EditLibrary(_m->name, _m->path, this);

	connect(edit_dialog, &GUI_EditLibrary::sig_accepted, this, &LocalLibraryMenu::edit_accepted);

	edit_dialog->show();
}

void LocalLibraryMenu::edit_accepted()
{
	GUI_EditLibrary* edit_dialog = static_cast<GUI_EditLibrary*>(sender());
	QString name = edit_dialog->name();
	QString path = edit_dialog->path();

	if(name.isEmpty() || path.isEmpty())
	{
		return;
	}

	if(edit_dialog->has_name_changed()){
		emit sig_name_changed(name);
	}

	if(edit_dialog->has_path_changed()){
		emit sig_path_changed(path);
	}
}

void LocalLibraryMenu::show_album_cover_view_changed()
{
	_settings->set(Set::Lib_ShowAlbumCovers, _m->show_album_cover_view->isChecked());
}

// TODO: This has nothing to do with GUI.
// TODO: MOve database operations to LocalLibrary
void LocalLibraryMenu::show_album_artists_changed()
{
	bool show_album_artist = _m->show_album_artists_action->isChecked();
	_settings->set(Set::Lib_ShowAlbumArtists, show_album_artist);

	QList<LibraryDatabase*> dbs = DatabaseConnector::getInstance()->library_dbs();
	for(LibraryDatabase* lib_db : dbs){
		if(lib_db->db_id() == 0){
			if(show_album_artist){
				lib_db->change_artistid_field(LibraryDatabase::ArtistIDField::AlbumArtistID);
			}

			else{
				lib_db->change_artistid_field(LibraryDatabase::ArtistIDField::ArtistID);
			}
		}
	}

	emit sig_show_album_artists_changed();
}

