/* GUI_LastFM.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_LastFM.cpp
 *
 *  Created on: Apr 21, 2011
 *      Author: luke
 */

#include "GUI_LastFM.h"
#include "Helper/Helper.h"
#include "Components/StreamPlugins/LastFM/LastFM.h"


GUI_LastFM::GUI_LastFM(QWidget* parent) :
	PreferenceDialogInterface(parent),
	Ui_GUI_LastFM_Dialog()
{
	_lfm = LastFM::getInstance();
}


GUI_LastFM::~GUI_LastFM() {

}


void GUI_LastFM::init_ui()
{
	setup_parent(this);

	bool active = _settings->get(Set::LFM_Active);

	cb_activate->setChecked(active);
	active_changed(active);


	logged_in(_lfm->is_logged_in());


	StringPair user_pw = _settings->get(Set::LFM_Login);
	tf_username->setText(user_pw.first);
	tf_password->setText(user_pw.second);

	connect(btn_ok, &QPushButton::clicked, this, &GUI_LastFM::btn_ok_clicked);
	connect(btn_login, &QPushButton::clicked, this, &GUI_LastFM::btn_login_clicked);
	connect(cb_activate, &QCheckBox::toggled, this, &GUI_LastFM::active_changed);
	connect(_lfm, &LastFM::sig_last_fm_logged_in, this, &GUI_LastFM::logged_in);
}



void GUI_LastFM::language_changed() {

	PreferenceDialogInterface::language_changed();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_LastFM::btn_login_clicked(){

	if(tf_username->text().length() < 3) {
		return;
	}

	if(tf_password->text().length() < 3) {
		return;
	}

	StringPair user_pw;
	user_pw.first = tf_username->text();
	user_pw.second = tf_password->text();

	_settings->set(Set::LFM_Login, user_pw);

	_lfm->psl_login();
}


void GUI_LastFM::btn_ok_clicked() {

	StringPair user_pw;
	user_pw.first = tf_username->text();
	user_pw.second = tf_password->text();

	_settings->set(Set::LFM_Login, user_pw);

	if( tf_username->text().length() >= 3 &&
		tf_password->text().length() >= 3 )
	{
		_lfm->psl_login();

		_settings->set( Set::LFM_Active, cb_activate->isChecked() );
	}

	close();
}


void GUI_LastFM::active_changed(bool active) {

	if(!is_ui_initialized()){
		return;
	}

	tf_username->setEnabled(active);
	tf_password->setEnabled(active);
}



void GUI_LastFM::logged_in(bool success){

	if(!is_ui_initialized()){
		return;
	}

	if(success){
		lab_status->setText(tr("Logged in"));
	}

	else{
		lab_status->setText(tr("Not logged in"));
	}
}


QString GUI_LastFM::get_action_name() const
{
	return tr("Last.fm");
}

