/* PreferenceDialog.cpp */

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



#include "PreferenceDialogInterface.h"


PreferenceDialogInterface::PreferenceDialogInterface(QWidget* parent) :
	SayonaraDialog(parent)
{
	_is_initialized = false;
}

PreferenceDialogInterface::~PreferenceDialogInterface()
{

}

void PreferenceDialogInterface::language_changed()
{
	QString new_name = this->get_action_name();
	this->get_action()->setText(new_name + "...");

	if(!is_ui_initialized()){
		return;
	}

	QLabel* label = get_title_label();
	if(label){
		label->setText(new_name);
	}

	this->setWindowTitle(new_name);
}


void PreferenceDialogInterface::showEvent(QShowEvent* e)
{
	if(!is_ui_initialized()){
		init_ui();
	}

	SayonaraDialog::showEvent(e);

	if(!_geometry.isEmpty()){
		this->restoreGeometry(_geometry);
	}
}

void PreferenceDialogInterface::closeEvent(QCloseEvent* e)
{
	_geometry = saveGeometry();

	SayonaraDialog::closeEvent(e);
}


QAction* PreferenceDialogInterface::get_action(){

	// action has to be initialized here, because pure
	// virtual get_action_name should not be called from ctor
	QString name = get_action_name();
	if(!_action){
		_action = new QAction(name + "...", nullptr);
		connect(_action, &QAction::triggered, this, &PreferenceDialogInterface::show);
	}

	_action->setText(name + "...");
	return _action;
}



bool PreferenceDialogInterface::is_ui_initialized() const
{
	return _is_initialized;
}

