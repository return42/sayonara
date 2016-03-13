/* GUIRemoteControl.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 3, 2012 
 *
 */


#include "GUI_RemoteControl.h"


GUI_RemoteControl::GUI_RemoteControl(QWidget* parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_RemoteControl()
{

}

GUI_RemoteControl::~GUI_RemoteControl() {

}

void GUI_RemoteControl::init_ui()
{
	setup_parent(this);

	cb_activate->setChecked(_settings->get(Set::Remote_Active));
	sb_port->setValue(_settings->get(Set::Remote_Port));

	connect(bb_ok_cancel, &QDialogButtonBox::accepted, this, &GUI_RemoteControl::ok_clicked);
	connect(bb_ok_cancel, &QDialogButtonBox::rejected, this, &GUI_RemoteControl::cancel_clicked);
}


void GUI_RemoteControl::language_changed() {

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceDialogInterface::language_changed();
}

void GUI_RemoteControl::ok_clicked(){

	_settings->set(Set::Remote_Active, cb_activate->isChecked());
	_settings->set(Set::Remote_Port, sb_port->value());
}

void GUI_RemoteControl::cancel_clicked()
{
	cb_activate->setChecked(_settings->get(Set::Remote_Active));
	sb_port->setValue(_settings->get(Set::Remote_Port));
	this->close();
}

QString GUI_RemoteControl::get_action_name() const
{
	return tr("Remote control");
}



QLabel*GUI_RemoteControl::get_title_label()
{
	return lab_title;
}
