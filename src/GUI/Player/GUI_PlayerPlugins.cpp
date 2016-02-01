/* GUI_PlayerPlugins.cpp */

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

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"

/** PLUGINS **/
void GUI_Player::hide_all_plugins() {

	_settings->set(Set::Player_ShownPlugin, QString(""));

	if(plugin_widget->isHidden()) {
		return;
	}

	int old_height;
	QSize pl_size;

	old_height = plugin_widget->minimumHeight();
	pl_size = ui_playlist->size();
	pl_size.setHeight(pl_size.height() + old_height);

	plugin_widget->setMinimumHeight(0);
}


void GUI_Player::show_plugin(PlayerPluginInterface* plugin) {

	hide_all_plugins();

    if(!plugin) return;

	int old_height;
	QSize pl_size, widget_size;

	plugin_widget->show();

    _pph->showPlugin(plugin);

	widget_size = plugin->get_size();
	widget_size.setWidth( plugin_widget->width() );

	pl_size = ui_playlist->size();
	old_height = this->plugin_widget->minimumHeight();
	pl_size.setHeight(pl_size.height() - widget_size.height() + old_height);

	plugin_widget->setMinimumHeight(plugin->get_size().height());

	_pph->resize(plugin_widget->size());

	_settings->set(Set::Player_ShownPlugin, plugin->get_name());
}



/** PLUGINS **/
