/* GUI_StartupDialog.cpp */

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



#include "GUI_StartupDialog.h"

GUI_StartupDialog::GUI_StartupDialog(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_StartupDialog()
{

}

GUI_StartupDialog::~GUI_StartupDialog() {

}

void GUI_StartupDialog::init_ui()
{
	setup_parent(this);

	bool load_saved_playlists, load_temporary_playlists, load_last_track, remember_time, start_playing;

	load_saved_playlists = _settings->get(Set::PL_LoadSavedPlaylists);
	load_temporary_playlists = _settings->get(Set::PL_LoadTemporaryPlaylists);
	load_last_track = _settings->get(Set::PL_LoadLastTrack);
	remember_time = _settings->get(Set::PL_RememberTime);
	start_playing = _settings->get(Set::PL_StartPlaying);

	cb_load_saved_playlists->setChecked(load_saved_playlists);
	cb_load_temporary_playlists->setChecked(load_temporary_playlists);
	cb_load_last_track->setChecked(load_last_track);
	cb_remember_time->setChecked(remember_time);
	cb_start_playing->setChecked(start_playing);


	cb_toggled(true);

	connect(cb_load_last_track, &QCheckBox::toggled, this, &GUI_StartupDialog::cb_toggled);
	connect(cb_load_saved_playlists, &QCheckBox::toggled, this, &GUI_StartupDialog::cb_toggled);
	connect(cb_load_temporary_playlists, &QCheckBox::toggled, this, &GUI_StartupDialog::cb_toggled);
	connect(cb_remember_time, &QCheckBox::toggled, this, &GUI_StartupDialog::cb_toggled);
	connect(cb_start_playing, &QCheckBox::toggled, this, &GUI_StartupDialog::cb_toggled);
	connect(btn_ok, &QPushButton::clicked, this, &GUI_StartupDialog::ok_clicked);
}

void GUI_StartupDialog::language_changed() {

	PreferenceDialogInterface::language_changed();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_StartupDialog::cb_toggled(bool b) {
    Q_UNUSED(b);

	bool load = (cb_load_saved_playlists->isChecked() || cb_load_temporary_playlists->isChecked());

	cb_load_last_track->setEnabled(load);
	cb_remember_time->setEnabled(load);
	cb_start_playing->setEnabled(load);

	bool cb_load_last_track_checked = cb_load_last_track->isChecked() && cb_load_last_track->isEnabled();
	cb_remember_time->setEnabled(cb_load_last_track_checked);

}

void GUI_StartupDialog::ok_clicked() {

	_settings->set( Set::PL_LoadSavedPlaylists, cb_load_saved_playlists->isChecked() );
	_settings->set( Set::PL_LoadTemporaryPlaylists, cb_load_temporary_playlists->isChecked() );
	_settings->set( Set::PL_LoadLastTrack, (cb_load_last_track->isChecked() && cb_load_last_track->isEnabled()) );
	_settings->set( Set::PL_RememberTime, (cb_remember_time->isChecked() && cb_remember_time->isEnabled()) );
	_settings->set( Set::PL_StartPlaying, (cb_start_playing->isChecked() && cb_start_playing->isEnabled()) );

    close();
}

QString GUI_StartupDialog::get_action_name() const
{
	return tr("Start up");
}
