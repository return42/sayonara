/* PlayerPluginHandler.cpp */

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

#include "PlayerPlugin.h"
#include "PlayerPluginHandler.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Settings/Settings.h"

#include <QAction>


PlayerPluginHandler::PlayerPluginHandler(QObject *parent) :
	QObject(parent),
	SayonaraClass()
{
	Set::listen(Set::Player_Language, this, &PlayerPluginHandler::language_changed);
}

PlayerPluginHandler::~PlayerPluginHandler() {}

PlayerPluginInterface* PlayerPluginHandler::find_plugin(const QString& name) 
{
	sp_log(Log::Debug, this) << "Search for plugin " << name;

	for(PlayerPluginInterface* p : _plugins) {
		if(p->get_name().compare(name) == 0){
			return p;
		}
	}	

	return nullptr;
}


void PlayerPluginHandler::add_plugin(PlayerPluginInterface* p) 
{
	if(!p){
		return;
	}

	_plugins.push_back(p);

	connect(p, &PlayerPluginInterface::sig_action_triggered, this, &PlayerPluginHandler::plugin_action_triggered);
	connect(p, &PlayerPluginInterface::sig_reload, this,  &PlayerPluginHandler::reload_plugin);
	connect(p, &PlayerPluginInterface::sig_closed, this, &PlayerPluginHandler::plugin_closed);

	QString last_plugin = _settings->get(Set::Player_ShownPlugin);
	if(p->get_name() == last_plugin){
		p->get_action()->setChecked(true);
		plugin_action_triggered(p, true);
	}
}


void PlayerPluginHandler::plugin_action_triggered(PlayerPluginInterface* p, bool b) 
{
	if(!p){
		return;
	}

	if(b) {
		emit sig_show_plugin(p);
		_settings->set(Set::Player_ShownPlugin, p->get_name());
	}

	else {
		plugin_closed();
	}
}


void PlayerPluginHandler::plugin_closed()
{
	emit sig_hide_all_plugins();
	_settings->set(Set::Player_ShownPlugin, QString());
}


void PlayerPluginHandler::reload_plugin(PlayerPluginInterface* p) 
{
	if(p) {
		emit sig_show_plugin(p);
	}
}


void PlayerPluginHandler::language_changed()
{
	for(PlayerPluginInterface* p : _plugins) {
		p->language_changed();
		p->get_action()->setText(p->get_display_name());
	}
}


QList<PlayerPluginInterface*> PlayerPluginHandler::get_all_plugins() const 
{
	return _plugins;
}
