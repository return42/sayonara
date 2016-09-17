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

	cb_active->setChecked( _settings->get(Set::Engine_SpeedActive));
	cb_preserve_pitch->setEnabled( _settings->get(Set::Engine_SpeedActive));
	cb_preserve_pitch->setChecked( _settings->get(Set::Engine_PreservePitch));

	sli_speed->setEnabled( _settings->get(Set::Engine_SpeedActive));
	sli_speed->setValue(_settings->get(Set::Engine_Speed) * 100);
	lab_speed->setText( QString::number(_settings->get(Set::Engine_Speed)));

	sb_concert_pitch->setEnabled( !_settings->get(Set::Engine_SpeedActive));
	sb_concert_pitch->setValue( _settings->get(Set::Engine_Pitch));

	connect(sli_speed, &QSlider::valueChanged, this, &GUI_Speed::slider_changed);
	connect(cb_active, &QCheckBox::toggled, this, &GUI_Speed::active_changed);
	connect(cb_preserve_pitch, &QCheckBox::toggled, this, &GUI_Speed::preserve_pitch_changed);
	connect(sb_concert_pitch, SIGNAL(valueChanged(int)), this, SLOT(pitch_changed(int)));
}


QString GUI_Speed::get_name() const
{
	return "Speed";
}

QString GUI_Speed::get_display_name() const
{
	return tr("Speed");
}


void GUI_Speed::slider_changed(int val) {

	float val_f = val / 100.0f;
	lab_speed->setText(QString::number(val_f, 'f', 2));

	if( !cb_active->isChecked() ) return;

	_settings->set(Set::Engine_Speed, sli_speed->value() / 100.0f);
}


void GUI_Speed::active_changed(bool b) {

	sli_speed->setEnabled(b);
	cb_preserve_pitch->setEnabled(b);
	sb_concert_pitch->setDisabled(b);
	_settings->set(Set::Engine_SpeedActive, b);

	if(!b) {
		_settings->set(Set::Engine_Speed, 1.0f);
	}

	else {
		_settings->set(Set::Engine_Speed, sli_speed->value() / 100.0f );
	}
}

void GUI_Speed::preserve_pitch_changed(bool enabled)
{
	_settings->set(Set::Engine_PreservePitch, enabled);
}

void GUI_Speed::pitch_changed(int value)
{
	_settings->set(Set::Engine_Pitch, value);
}
