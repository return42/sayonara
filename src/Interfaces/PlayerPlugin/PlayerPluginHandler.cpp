/* PlayerPluginHandler.cpp */

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


#include "PlayerPlugin.h"
#include "PlayerPluginHandler.h"
#include "Helper/Logger/Logger.h"


#include <QMap>
#include <QString>
#include <QAction>


PlayerPluginHandler::PlayerPluginHandler(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
    _cur_shown_plugin = nullptr;

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

PlayerPluginHandler::~PlayerPluginHandler() {}


PlayerPluginInterface* PlayerPluginHandler::find_plugin(QString name) {

	sp_log(Log::Debug) << "Search for plugin " << name;

	for(PlayerPluginInterface* p : _plugins) {
		if(p->get_name().compare(name) == 0){
			return p;
		}
	}	

    return nullptr;
}

void PlayerPluginHandler::addPlugin(PlayerPluginInterface* p) {

    _plugins.push_back(p);
	connect(p, &PlayerPluginInterface::sig_action_triggered, this, &PlayerPluginHandler::plugin_action_triggered);
	connect(p, &PlayerPluginInterface::sig_reload, this,  &PlayerPluginHandler::reload_plugin);
}


void PlayerPluginHandler::plugin_action_triggered(PlayerPluginInterface* p, bool b) {

    if(b) {
        emit sig_show_plugin(p);
    }

    else{
	
        hide_all();
        _cur_shown_plugin = nullptr;
    }
}


void PlayerPluginHandler::showPlugin(PlayerPluginInterface* p) {

     hide_all();

	 QAction* action;

	 action = p->get_action();

	 if(action){
		 action->setChecked(true);
	 }

	 p->show();

     _cur_shown_plugin = p;
}


void PlayerPluginHandler::reload_plugin(PlayerPluginInterface* p) {
	if(p) {
		plugin_action_triggered(p, true);
	}
}

void PlayerPluginHandler::language_changed()
{
	for(PlayerPluginInterface* p : _plugins) {
		p->language_changed();
		p->get_action()->setText(p->get_display_name());
	}
}

void PlayerPluginHandler::hide_all() {

   _cur_shown_plugin = nullptr;

   for(PlayerPluginInterface* p : _plugins) {
	   if(!p->isHidden()){
			p->close();
	   }
   }

   emit sig_hide_all_plugins();
}


void PlayerPluginHandler::resize(QSize sz) {

    if(!_cur_shown_plugin) return;
    _cur_shown_plugin->resize(sz);
}

QList<PlayerPluginInterface*> PlayerPluginHandler::get_all_plugins() const {
	return _plugins;
}

