/* GUI_PlayerPreferences.cpp */

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

#include "GUI_PlayerPreferences.h"
#include "GUI/Preferences/ui_GUI_PlayerPreferences.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"


GUI_PlayerPreferences::GUI_PlayerPreferences(const QString& identifier) :
	Base(identifier) {}

GUI_PlayerPreferences::~GUI_PlayerPreferences()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}


void GUI_PlayerPreferences::init_ui()
{
	setup_parent(this, &ui);

	revert();

	connect(ui->cb_show_tray_icon, &QCheckBox::toggled, this, &GUI_PlayerPreferences::show_tray_icon_toggled);
}


QString GUI_PlayerPreferences::action_name() const
{
	return Lang::get(Lang::Application);
}


bool GUI_PlayerPreferences::commit()
{
	bool show_tray_icon = _settings->get(Set::Player_ShowTrayIcon);

	_settings->set(Set::Player_Min2Tray, ui->cb_close_to_tray->isChecked() && show_tray_icon);
	_settings->set(Set::Player_StartInTray, ui->cb_start_in_tray->isChecked() && show_tray_icon);

	_settings->set(Set::Player_ShowTrayIcon, ui->cb_show_tray_icon->isChecked());
	_settings->set(Set::Player_NotifyNewVersion, ui->cb_update_notifications->isChecked());

	return true;
}

void GUI_PlayerPreferences::revert()
{
	bool show_tray_icon = _settings->get(Set::Player_ShowTrayIcon);

	ui->cb_start_in_tray->setChecked(_settings->get(Set::Player_StartInTray));
	ui->cb_close_to_tray->setChecked(_settings->get(Set::Player_Min2Tray));
	ui->cb_update_notifications->setChecked(_settings->get(Set::Player_NotifyNewVersion));
	ui->cb_show_tray_icon->setChecked(_settings->get(Set::Player_ShowTrayIcon));

	show_tray_icon_toggled(show_tray_icon);
}

void GUI_PlayerPreferences::show_tray_icon_toggled(bool b)
{
	ui->cb_start_in_tray->setEnabled(b);
	ui->cb_close_to_tray->setEnabled(b);

	if(!b){
		_settings->set(Set::Player_Min2Tray, false);
	}
}

void GUI_PlayerPreferences::retranslate_ui()
{
	ui->retranslateUi(this);
}
