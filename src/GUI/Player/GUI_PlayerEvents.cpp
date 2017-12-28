/* GUI_PlayerEvents.cpp */

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

#include "GUI_Player.h"
#include "Utils/Settings/Settings.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

#include <QShowEvent>
#include <QCloseEvent>

void GUI_Player::moveEvent(QMoveEvent *e)
{
	QMainWindow::moveEvent(e);

	QPoint p= this->pos();
	_settings->set(Set::Player_Pos, p);
}


void GUI_Player::resizeEvent(QResizeEvent* e)
{
	QMainWindow::resizeEvent(e);
	Library::PluginHandler* lph = Library::PluginHandler::instance();

	bool is_maximized = _settings->get(Set::Player_Maximized);
	bool is_fullscreen = _settings->get(Set::Player_Fullscreen);
	bool is_library_visible = _settings->get(Set::Lib_Show);

	if(is_maximized) {
		_settings->set(Set::Player_Fullscreen, false);
	}

	if(is_library_visible && lph){
		Library::Container* container;
		container = lph->current_library();
		if(container && container->is_initialized()){
			container->widget()->resize(library_widget->size());
		}
	}

	if( !is_maximized &&
		!this->isMaximized() &&
		!is_fullscreen &&
		!this->isFullScreen())
	{
		_settings->set(Set::Player_Size, this->size());
	}

	update();
}


void GUI_Player::main_splitter_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	if(lph)
	{
		Library::Container* container;
		container = lph->current_library();
		if(container){
			container->widget()->resize(library_widget->size());
		}
	}

	QByteArray splitter_state = splitter->saveState();
	_settings->set(Set::Player_SplitterState, splitter_state);
}


void GUI_Player::closeEvent(QCloseEvent* e)
{
	e->ignore();

	bool min_to_tray = _settings->get(Set::Player_Min2Tray);

	if(min_to_tray && !this->isHidden()) {
		this->hide();
	}

	_settings->set(Set::Player_Maximized, this->isMaximized());
	_settings->set(Set::Player_Fullscreen, this->isFullScreen());
	_settings->set(Set::Player_Pos, this->pos());

	if(!min_to_tray){
		really_close();
	}
}
