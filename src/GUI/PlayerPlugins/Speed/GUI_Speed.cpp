/* GUI_Speed.cpp */

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



#include "GUI_Speed.h"

#include "Components/Engine/EngineHandler.h"

GUI_Speed::GUI_Speed(QWidget *parent) :
	PlayerPluginInterface(parent),
	Ui::GUI_Speed()
{

}

void GUI_Speed::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
}

void GUI_Speed::init_ui()
{
	setup_parent(this);

	_engine = EngineHandler::getInstance();

	connect(sli_speed, &QSlider::valueChanged, this, &GUI_Speed::slider_changed);
	connect(cb_active, &QCheckBox::toggled, this, &GUI_Speed::active_changed);
}


QString GUI_Speed::get_name() const
{
	return "Speed";
}

QString GUI_Speed::get_display_name() const
{
	return tr("Speed");
}

QLabel* GUI_Speed::get_title_label() const
{
	return lab_title;
}

QPushButton* GUI_Speed::get_close_button() const
{
	return btn_close;
}

void GUI_Speed::slider_changed(int val) {

	float val_f = val / 100.0f;
	lab_speed->setText(QString::number(val_f, 'f', 2));

	if( !cb_active->isChecked() ) return;

	_engine->set_speed(val_f);
}


void GUI_Speed::active_changed(bool b) {

	if(!b) {
		_engine->set_speed(1.0f);
	}

	else {
		_engine->set_speed( sli_speed->value() / 100.0f );
	}
}
