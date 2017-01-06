/* GUI_Crossfader.cpp */

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


#include "GUI_Crossfader.h"
#include "GUI/Plugins/Engine/ui_GUI_Crossfader.h"
#include "Helper/Playlist/PlaylistMode.h"


GUI_Crossfader::GUI_Crossfader(QWidget *parent) :
	PlayerPluginInterface(parent)
{

}

GUI_Crossfader::~GUI_Crossfader()
{
	if(ui)
	{
		delete ui; ui = nullptr;
	}
}


void GUI_Crossfader::init_ui()
{
	setup_parent(this, &ui);

	int val = _settings->get(Set::Engine_CrossFaderTime);
	bool enabled = _settings->get(Set::Engine_CrossFaderActive);

	ui->cb_active->setChecked(enabled);
	ui->sli_crossfader->setEnabled(enabled);
	ui->sli_crossfader->setValue(val);

	ui->lab_crossfader->setText(QString::number(val) + " ms");

	connect(ui->sli_crossfader, &QSlider::valueChanged, this, &GUI_Crossfader::slider_changed);
	connect(ui->cb_active, &QCheckBox::toggled, this, &GUI_Crossfader::active_changed);
}


void GUI_Crossfader::language_changed(){

	if(!is_ui_initialized()){
		return;
	}

	ui->retranslateUi(this);
}


QString GUI_Crossfader::get_name() const
{
	return "Crossfader";
}


QString GUI_Crossfader::get_display_name() const
{
	return tr("Crossfader");
}


void GUI_Crossfader::slider_changed(int val)
{
	ui->lab_crossfader->setText(QString::number(val) + "ms");
	_settings->set(Set::Engine_CrossFaderTime, val);
}


void GUI_Crossfader::active_changed(bool b)
{
	ui->sli_crossfader->setEnabled(b);

	Playlist::Mode plm = _settings->get(Set::PL_Mode);
	plm.setGapless(Playlist::Mode::isActive(plm.gapless()), !b);
	
	_settings->set(Set::PL_Mode, plm);
	_settings->set(Set::Engine_CrossFaderActive, b);
}
