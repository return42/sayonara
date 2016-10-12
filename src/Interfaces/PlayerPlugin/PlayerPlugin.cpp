/* PlayerPlugin.cpp */

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

#include "GUI/Helper/Shortcuts/Shortcut.h"

#include "GUI/Helper/Shortcuts/ShortcutHandler.h"
#include "Components/PlayManager/PlayManager.h"

#include <QLayout>

PlayerPluginInterface::PlayerPluginInterface(QWidget *parent) :
	SayonaraWidget(parent),
	ShortcutWidget()
{
	_play_manager = PlayManager::getInstance();
	_is_initialized = false;

	_pp_action = new QAction(nullptr);
	_pp_action->setCheckable(true);

	connect(_pp_action, &QAction::triggered, this, &PlayerPluginInterface::action_triggered);
	connect(_play_manager, &PlayManager::sig_playstate_changed,	this, &PlayerPluginInterface::playstate_changed);

	hide();
}


PlayerPluginInterface::~PlayerPluginInterface() {

	delete _pp_action; _pp_action = nullptr;
}

void PlayerPluginInterface::show(){
	QWidget::show();

	init_ui();
}

bool PlayerPluginInterface::is_title_shown() const
{
	return true;
}

QString PlayerPluginInterface::get_shortcut_text(const QString& shortcut_identifier) const
{
	if(shortcut_identifier == "close_plugin"){
		return tr("Close plugin");
	}

	return "";
}


QAction* PlayerPluginInterface::get_action() const {

	_pp_action->setText( this->get_display_name() );
	return _pp_action;
}


QSize PlayerPluginInterface::get_size() const
{
	return this->minimumSize();
}


void PlayerPluginInterface::finalize_initialization()
{
	QLayout* widget_layout = layout();
	if(widget_layout){
		widget_layout->setContentsMargins(3, 3, 3, 3);
	}

	ShortcutHandler* sch = ShortcutHandler::getInstance();
	Shortcut sc = sch->get_shortcut("close_plugin");
	if(!sc.is_valid()){
		sc = sch->add(Shortcut(this, "close_plugin", tr("Close plugin"), "Ctrl+Esc"));
	}

	sc.create_qt_shortcut(this, this, SLOT(close()));

	REGISTER_LISTENER(Set::Player_Language, _sl_lang_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);

	this->set_ui_initialized();
}

void PlayerPluginInterface::closeEvent(QCloseEvent* e) {
	SayonaraWidget::closeEvent(e);

    _pp_action->setChecked(false);
}

bool PlayerPluginInterface::is_ui_initialized() const
{
	return _is_initialized;
}

void PlayerPluginInterface::set_ui_initialized()
{
	_is_initialized = true;
}


void PlayerPluginInterface::action_triggered(bool b) {

	_pp_action->setChecked(b);

	skin_changed();

	emit sig_action_triggered(this, b);
}

void PlayerPluginInterface::_sl_lang_changed()
{
	if(!is_ui_initialized()){
		return;
	}
}


void PlayerPluginInterface::playstate_changed(PlayState state){

	switch(state){
		case PlayState::Playing:
			played();
			break;

		case PlayState::Paused:
			paused();
			break;

		case PlayState::Stopped:
			stopped();
			break;

		default:
			return;
	}
}

void PlayerPluginInterface::played(){

}

void PlayerPluginInterface::paused(){

}

void PlayerPluginInterface::stopped(){

}
