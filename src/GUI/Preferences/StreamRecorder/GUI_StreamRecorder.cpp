/* GUI_StreamRecorder.cpp

 * Copyright (C) 2011-2017 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * May 13, 2012 
 *
 */

#include "GUI_StreamRecorder.h"
#include "GUI/Preferences/ui_GUI_StreamRecorder.h"

#include "Database/DatabaseConnector.h"
#include "Helper/Message/Message.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include <QFileDialog>
#include <QDir>

GUI_StreamRecorder::GUI_StreamRecorder(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{

}


GUI_StreamRecorder::~GUI_StreamRecorder()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

void GUI_StreamRecorder::init_ui()
{
	setup_parent(this, &ui);

	revert();

	connect(ui->cb_activate, &QCheckBox::toggled, this, &GUI_StreamRecorder::sl_cb_activate_toggled);
	connect(ui->btn_path, &QPushButton::clicked, this, &GUI_StreamRecorder::sl_btn_path_clicked);
}

void GUI_StreamRecorder::retranslate_ui()
{
	ui->retranslateUi(this);
}


void GUI_StreamRecorder::sl_cb_activate_toggled(bool b)
{
	ui->le_path->setEnabled(b);
	ui->btn_path->setEnabled(b);
	ui->cb_auto_rec->setEnabled(b);
	ui->cb_create_session_path->setEnabled(b);
}


void GUI_StreamRecorder::sl_btn_path_clicked()
{
	QString path = ui->cb_create_session_path->text();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose target directory"), path, QFileDialog::ShowDirsOnly);
	if(dir.size() > 0) {
		ui->le_path->setText(path);
	}
}


void GUI_StreamRecorder::commit()
{
	_settings->set(Set::Engine_SR_Active, ui->cb_activate->isChecked());

	if(!ui->le_path->isEnabled()){
		return;
	}

	QString str = ui->le_path->text();
	if(!QFile::exists(str))
	{
        bool create_success = QDir::root().mkpath(str);
		if(!create_success)
		{
			QString sr_path = _settings->get(Set::Engine_SR_Path);
			ui->le_path->setText(sr_path);

			Message::warning(str + tr(" could not be created\nPlease choose another folder"), tr("Stream recorder"));
        }
    }

	_settings->set(Set::Engine_SR_Path, str);
	_settings->set(Set::Engine_SR_AutoRecord, ui->cb_auto_rec->isChecked());
	_settings->set(Set::Engine_SR_SessionPath, ui->cb_create_session_path->isChecked());
}

void GUI_StreamRecorder::revert()
{
	bool lame_available = _settings->get(SetNoDB::MP3enc_found);

	QString path = _settings->get(Set::Engine_SR_Path);
	bool active = _settings->get(Set::Engine_SR_Active) && lame_available;
	bool create_session_path = _settings->get(Set::Engine_SR_SessionPath);
	bool auto_rec = _settings->get(Set::Engine_SR_AutoRecord);

	ui->cb_activate->setEnabled(lame_available);
	ui->le_path->setText(path);
	ui->cb_activate->setChecked(active);
	ui->cb_create_session_path->setChecked(create_session_path);
	ui->cb_auto_rec->setChecked(auto_rec);

	ui->cb_create_session_path->setEnabled(active);
	ui->btn_path->setEnabled(active);
	ui->le_path->setEnabled(active);
	ui->cb_auto_rec->setEnabled(active);

	if(!lame_available){
		ui->lab_warning->setText(Lang::get(Lang::CannotFindLame));
	}
	else{
		ui->lab_warning->clear();
	}
}


QString GUI_StreamRecorder::get_action_name() const
{
	return tr("Stream recorder");
}
