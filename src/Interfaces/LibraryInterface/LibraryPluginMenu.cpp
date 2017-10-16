/* LibraryPluginMenu.cpp */

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



#include "LibraryPluginMenu.h"
#include "LibraryPluginHandler.h"
#include "LibraryContainer/LibraryContainer.h"
#include "Helper/Settings/Settings.h"

#include <QList>
#include <QAction>

struct LibraryPluginMenu::Private
{
	LibraryPluginHandler* lph=nullptr;
	QList<QAction*> actions;

	Private()
	{
		lph = LibraryPluginHandler::getInstance();
	}
};

LibraryPluginMenu::LibraryPluginMenu(QWidget* parent) :
    QMenu(parent),
    SayonaraClass()
{
	_m = Pimpl::make<Private>();

	connect(_m->lph, &LibraryPluginHandler::sig_initialized, this, &LibraryPluginMenu::setup_actions);
	connect(_m->lph, &LibraryPluginHandler::sig_current_library_changed, this, &LibraryPluginMenu::current_library_changed);

	Set::listen(Set::Lib_AllLibraries, this, &LibraryPluginMenu::setup_actions, false);
	Set::listen(Set::Player_Language, this, &LibraryPluginMenu::setup_actions, false);
}

LibraryPluginMenu::~LibraryPluginMenu() {}


void LibraryPluginMenu::setup_actions()
{
	this->clear();
	_m->actions.clear();

	bool separator_inserted = false;
	LibraryContainerInterface* current_library = _m->lph->current_library();
	QList<LibraryContainerInterface*> libraries = _m->lph->get_libraries();

	for(const LibraryContainerInterface* container : libraries)
	{
		if(!LibraryPluginHandler::is_local_library(container) && !separator_inserted){
			this->addSeparator();
			separator_inserted = true;
		}

		QAction* action = new QAction(container->display_name(), this);
		QString name = container->name();

		action->setCheckable(true);
		action->setChecked(false);
		action->setData(container->name());

		_m->actions << action;

		connect(action, &QAction::triggered, this, &LibraryPluginMenu::action_triggered);

		this->addAction(action);
		if(current_library && current_library->name().compare(name) == 0){
			action->trigger();
		}
	}
}

void LibraryPluginMenu::action_triggered(bool b)
{
	if(!b){
		return;
	}

	QAction* action = static_cast<QAction*>(sender());
	QString name = action->data().toString();

	LibraryPluginHandler::getInstance()->set_current_library(name);
	for(QAction* library_action : _m->actions)
	{
		if(library_action == action){
			continue;
		}

		library_action->setChecked(false);
	}
}

void LibraryPluginMenu::current_library_changed(const QString& name)
{
	for(QAction* action : _m->actions){
		QString action_name = action->data().toString();
		action->setChecked(action_name.compare(name) == 0);
	}
}
