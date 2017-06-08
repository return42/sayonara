/* LibraryContainer.cpp */

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

#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Helper/Settings/Settings.h"

#include <QAction>

struct LibraryContainerInterface::Private
{
	QAction*	action=nullptr;
	bool		initialized;

	Private()
	{
		initialized = false;
	}

	void set_initialized()
	{
		initialized = true;
	}

};

LibraryContainerInterface::LibraryContainerInterface(QObject* parent) :
	QObject(parent),
	SayonaraClass()
{
	_m = Pimpl::make<Private>();

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

LibraryContainerInterface::~LibraryContainerInterface() {}

QString LibraryContainerInterface::get_display_name() const
{
	return get_name();
}

QMenu* LibraryContainerInterface::get_menu()
{
	return nullptr;
}

void LibraryContainerInterface::set_menu_action(QAction* action)
{
	_m->action = action;
}

QAction* LibraryContainerInterface::get_menu_action() const
{
	return _m->action;
}

void LibraryContainerInterface::set_initialized()
{
	_m->set_initialized();
}

bool LibraryContainerInterface::is_initialized() const
{
	return _m->initialized;
}


void LibraryContainerInterface::language_changed()
{
	if(_m->action){
		_m->action->setText(this->get_display_name());
	}
}


