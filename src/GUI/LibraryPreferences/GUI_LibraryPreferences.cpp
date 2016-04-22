/* GUI_LibraryPreferences.cpp */

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



#include "GUI_LibraryPreferences.h"

GUI_LibraryPreferences::GUI_LibraryPreferences(QWidget* parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_LibraryPreferences()
{
}

void GUI_LibraryPreferences::init_ui(){

	setup_parent(this);

	rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));
}

QString GUI_LibraryPreferences::get_action_name() const
{
	return tr("Library-Playlist Interaction");
}

QLabel* GUI_LibraryPreferences::get_title_label()
{
	return lab_title;
}

void GUI_LibraryPreferences::commit(){
	_settings->set(Set::Lib_DC_DoNothing, rb_dc_do_nothing->isChecked());
	_settings->set(Set::Lib_DC_PlayIfStopped, rb_dc_play_if_stopped->isChecked());
	_settings->set(Set::Lib_DC_PlayImmediately, rb_dc_play_immediately->isChecked());
	_settings->set(Set::Lib_DD_DoNothing, rb_dd_do_nothing->isChecked());
	_settings->set(Set::Lib_DD_PlayIfStoppedAndEmpty, rb_dd_start_if_stopped_and_empty->isChecked());
}

void GUI_LibraryPreferences::revert(){

	rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));
}

void GUI_LibraryPreferences::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}
