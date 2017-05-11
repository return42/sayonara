/* LocalLibraryContainer.cpp */

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

#include "LocalLibraryContainer.h"
#include "GUI/Library/GUI_LocalLibrary.h"
#include "GUI/Helper/GUI_Helper.h"
#include "Helper/Library/LibraryInfo.h"

struct LocalLibraryContainer::Private
{
	GUI_LocalLibrary*   ui=nullptr;
	LibraryInfo			library;
	QString				name;
	QString				library_path;

	Private(const LibraryInfo& library) :
		library(library)
	{
		name = library.name();
	}
};

LocalLibraryContainer::LocalLibraryContainer(const LibraryInfo& library, QObject* parent) :
	LibraryContainerInterface(parent)
{
	_m = Pimpl::make<Private>(library);
}

LocalLibraryContainer::~LocalLibraryContainer()
{
	if(_m->ui) {
		delete _m->ui; _m->ui=nullptr;
	}
}

QString LocalLibraryContainer::get_name() const
{
	QString name = get_display_name();
	name = name.toLower();
	name.replace(" ", "-");
	return name;
}

QString LocalLibraryContainer::get_display_name() const
{
	return _m->name;
}

QIcon LocalLibraryContainer::get_icon() const
{
	return GUI::get_icon("append");
}

QWidget* LocalLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(_m->ui);
}

QComboBox* LocalLibraryContainer::get_libchooser()
{
	return _m->ui->get_libchooser();
}

QMenu*LocalLibraryContainer::get_menu()
{
	if(_m->ui){
		return _m->ui->get_menu();
	}

	return nullptr;
}

void LocalLibraryContainer::init_ui()
{
	_m->ui = new GUI_LocalLibrary(_m->library.id());
}

qint8 LocalLibraryContainer::get_id()
{
	return _m->library.id();
}

void LocalLibraryContainer::set_name(const QString& name)
{
	_m->name = name;
}
