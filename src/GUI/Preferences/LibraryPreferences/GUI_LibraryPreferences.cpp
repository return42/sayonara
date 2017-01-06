/* GUI_LibraryPreferences.cpp */

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

#include "GUI_LibraryPreferences.h"
#include "GUI/Preferences/ui_GUI_LibraryPreferences.h"

#include "Helper/Library/SearchMode.h"
#include "Helper/Settings/Settings.h"

GUI_LibraryPreferences::GUI_LibraryPreferences(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{}

GUI_LibraryPreferences::~GUI_LibraryPreferences()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

void GUI_LibraryPreferences::init_ui(){

	setup_parent(this, &ui);

	revert();
}

QString GUI_LibraryPreferences::get_action_name() const
{
	return tr("Library");
}


void GUI_LibraryPreferences::commit()
{
	Library::SearchModeMask mask = 0;
	if(ui->cb_case_insensitive->isChecked()){
		mask |= Library::CaseInsensitve;
	}

	if(ui->cb_no_special_chars->isChecked()){
		mask |= Library::NoSpecialChars;
	}

	if(ui->cb_no_accents->isChecked()){
		mask |= Library::NoDiacriticChars;
	}

	_settings->set(Set::Lib_DC_DoNothing, ui->rb_dc_do_nothing->isChecked());
	_settings->set(Set::Lib_DC_PlayIfStopped, ui->rb_dc_play_if_stopped->isChecked());
	_settings->set(Set::Lib_DC_PlayImmediately, ui->rb_dc_play_immediately->isChecked());
	_settings->set(Set::Lib_DD_DoNothing, ui->rb_dd_do_nothing->isChecked());
	_settings->set(Set::Lib_DD_PlayIfStoppedAndEmpty, ui->rb_dd_start_if_stopped_and_empty->isChecked());
	_settings->set(Set::Lib_SearchMode, mask);
}

void GUI_LibraryPreferences::revert()
{
	ui->cb_case_insensitive->setChecked(false);
	ui->cb_no_special_chars->setChecked(false);
	ui->cb_no_accents->setChecked(false);

	Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);

	if(mask & Library::CaseInsensitve){
		ui->cb_case_insensitive->setChecked(true);
	}

	if(mask & Library::NoSpecialChars){
		ui->cb_no_special_chars->setChecked(true);
	}

	if(mask & Library::NoDiacriticChars){
		ui->cb_no_accents->setChecked(true);
	}

	ui->rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	ui->rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	ui->rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	ui->rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	ui->rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));
}

void GUI_LibraryPreferences::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	ui->retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}
