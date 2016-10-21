/* GUI_PlaylistPreferences.cpp */

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


/* GUI_PlaylistPreferences.cpp */

#include "GUI_PlaylistPreferences.h"
#include "GUI/Preferences/ui_GUI_PlaylistPreferences.h"

#include "GUI/Preferences/ui_GUI_PlaylistPreferences.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"


GUI_PlaylistPreferences::GUI_PlaylistPreferences(QWidget *parent) :
	PreferenceWidgetInterface(parent)
{

}

GUI_PlaylistPreferences::~GUI_PlaylistPreferences()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}



void GUI_PlaylistPreferences::commit()
{
	_settings->set( Set::PL_LoadSavedPlaylists, ui->cb_load_saved_playlists->isChecked() );
	_settings->set( Set::PL_LoadTemporaryPlaylists, ui->cb_load_temporary_playlists->isChecked() );
	_settings->set( Set::PL_LoadLastTrack, (ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled()) );
	_settings->set( Set::PL_RememberTime, (ui->cb_remember_time->isChecked() && ui->cb_remember_time->isEnabled()) );
	_settings->set( Set::PL_StartPlaying, (ui->cb_start_playing->isChecked() && ui->cb_start_playing->isEnabled()) );

	_settings->set(Set::PL_ShowNumbers, ui->cb_show_numbers->isChecked());
	_settings->set(Set::PL_EntryLook, ui->le_expression->text());
}

void GUI_PlaylistPreferences::revert()
{
	bool load_saved_playlists, load_temporary_playlists, load_last_track, remember_time, start_playing;

	load_saved_playlists = _settings->get(Set::PL_LoadSavedPlaylists);
	load_temporary_playlists = _settings->get(Set::PL_LoadTemporaryPlaylists);
	load_last_track = _settings->get(Set::PL_LoadLastTrack);
	remember_time = _settings->get(Set::PL_RememberTime);
	start_playing = _settings->get(Set::PL_StartPlaying);

	ui->cb_load_saved_playlists->setChecked(load_saved_playlists);
	ui->cb_load_temporary_playlists->setChecked(load_temporary_playlists);
	ui->cb_load_last_track->setChecked(load_last_track);
	ui->cb_remember_time->setChecked(remember_time);
	ui->cb_start_playing->setChecked(start_playing);

	ui->le_expression->setText(_settings->get(Set::PL_EntryLook));
	ui->cb_show_numbers->setChecked(_settings->get(Set::PL_ShowNumbers));
}

void GUI_PlaylistPreferences::init_ui()
{

	if(is_ui_initialized()){
		return;
	}

	setup_parent(this, &ui);

	revert();

	cb_toggled(true);

	connect(ui->cb_load_last_track, &QCheckBox::toggled, this, &GUI_PlaylistPreferences::cb_toggled);
	connect(ui->cb_load_saved_playlists, &QCheckBox::toggled, this, &GUI_PlaylistPreferences::cb_toggled);
	connect(ui->cb_load_temporary_playlists, &QCheckBox::toggled, this, &GUI_PlaylistPreferences::cb_toggled);
	connect(ui->cb_remember_time, &QCheckBox::toggled, this, &GUI_PlaylistPreferences::cb_toggled);
	connect(ui->cb_start_playing, &QCheckBox::toggled, this, &GUI_PlaylistPreferences::cb_toggled);

	connect(ui->btn_default, &QPushButton::clicked, [=](){
		ui->le_expression->setText("*%title%* - %artist%");
	});
}

QString GUI_PlaylistPreferences::get_action_name() const
{
	return Lang::get(Lang::Playlist);
}

void GUI_PlaylistPreferences::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	ui->retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}

void GUI_PlaylistPreferences::cb_toggled(bool b) {
	Q_UNUSED(b);

	bool load = (ui->cb_load_saved_playlists->isChecked() || ui->cb_load_temporary_playlists->isChecked());

	ui->cb_load_last_track->setEnabled(load);
	ui->cb_remember_time->setEnabled(load);
	ui->cb_start_playing->setEnabled(load);

	bool cb_load_last_track_checked = ui->cb_load_last_track->isChecked() && ui->cb_load_last_track->isEnabled();
	ui->cb_remember_time->setEnabled(cb_load_last_track_checked);

}
