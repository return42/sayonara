/* GUI_BroadcastSetup.cpp */

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



#include "GUI_BroadcastSetup.h"
#include "Helper/Helper.h"


GUI_BroadcastSetup::GUI_BroadcastSetup(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_BroadcastSetup()
{

}

GUI_BroadcastSetup::~GUI_BroadcastSetup(){

}

void GUI_BroadcastSetup::init_ui()
{
	setup_parent(this);
}

void GUI_BroadcastSetup::accept(){

	bool new_active = cb_active->isChecked();
	bool new_prompt = cb_prompt->isChecked();
	int new_port = sb_port->value();

	bool old_active = _settings->get(Set::Broadcast_Active);
	bool old_prompt = _settings->get(Set::Broadcast_Prompt);
	int old_port = _settings->get(Set::Broadcast_Port);

	if(old_active != new_active){
		_settings->set(Set::Broadcast_Active, new_active);
	}

	if(old_prompt != new_prompt){
		_settings->set(Set::Broadcast_Prompt, new_prompt);
	}

	if(old_port != new_port){
		_settings->set(Set::Broadcast_Port, new_port);
	}

	QDialog::accept();
}

void GUI_BroadcastSetup::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceDialogInterface::language_changed();
}

void GUI_BroadcastSetup::skin_changed()
{
	if(!is_ui_initialized()){
		return;
	}

	if(is_dark()){
		lab_icon->setPixmap(Helper::get_pixmap("broadcast"));
	}
	else{
		lab_icon->setPixmap(Helper::get_pixmap("broadcast_dark"));
	}
}

QLabel* GUI_BroadcastSetup::get_title_label()
{
	return lab_title;
}

void GUI_BroadcastSetup::reject(){
	QDialog::reject();
}

void GUI_BroadcastSetup::showEvent(QShowEvent* e){

	PreferenceDialogInterface::showEvent(e);

	if(is_dark()){
		lab_icon->setPixmap(Helper::get_pixmap("broadcast"));
	}
	else{
		lab_icon->setPixmap(Helper::get_pixmap("broadcast_dark"));
	}

	cb_active->setChecked( _settings->get(Set::Broadcast_Active) );
	cb_prompt->setChecked( _settings->get(Set::Broadcast_Prompt) );
	sb_port->setValue( _settings->get(Set::Broadcast_Port) );

	PreferenceDialogInterface::showEvent(e);
}

QString GUI_BroadcastSetup::get_action_name() const
{
	return tr("Broadcast");
}
