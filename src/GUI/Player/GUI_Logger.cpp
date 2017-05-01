/* GUI_Logger.cpp */

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



#include "GUI_Logger.h"
#include "GUI/Player/ui_GUI_Logger.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Helper/Message/Message.h"

#include <QStringList>
#include <QTextEdit>
#include <QThread>
#include <QFileDialog>
#include <QFile>
#include <QDir>

static LogObject log_object;

LogObject::LogObject(QObject* parent) :
	QObject(parent),
	LogListener()
{}

LogObject::~LogObject() {}

void LogObject::add_log_line(const QString& str)
{
	emit sig_new_log(str);
}


GUI_Logger::GUI_Logger(QWidget *parent) :
	SayonaraWidget(parent)
{
	connect(&log_object, &LogObject::sig_new_log,
			this, &GUI_Logger::log_ready, Qt::QueuedConnection);

	Logger::register_log_listener(this->get_log_listener());
}

GUI_Logger::~GUI_Logger()
{
    if(ui){
		delete ui; ui=nullptr;
    }
}

void GUI_Logger::init_ui()
{
	if(ui){
		return;
	}

	ui = new Ui::GUI_Logger;
	ui->setupUi(this);

	for(const QString& line : _buffer) {
		ui->te_log->appendPlainText(line);
	}

	_buffer.clear();

	connect(ui->btn_close, &QPushButton::clicked, this, &QWidget::close);
	connect(ui->btn_save, &QPushButton::clicked, this, &GUI_Logger::save_clicked);
}

void GUI_Logger::language_changed()
{
	if(ui){
		ui->retranslateUi(this);
	}
}


LogListener* GUI_Logger::get_log_listener()
{
	return &log_object;
}


void GUI_Logger::log_ready(const QString& str)
{
	if(!ui){
		_buffer << str;
	}

	else {
		ui->te_log->appendPlainText(str);
	}
}

void GUI_Logger::save_clicked()
{
	QString filename = QFileDialog::getSaveFileName(
						   this,
						   Lang::get(Lang::SaveAs),
						   QDir::homePath(), "*.log");

	QFile f(filename);
	bool is_open = f.open(QFile::WriteOnly);
	if(is_open){
		f.write(ui->te_log->toPlainText().toUtf8());
		f.close();
	}

	else {
		Message::warning(tr("Cannot open file") + " " + filename);
	}
}


void GUI_Logger::showEvent(QShowEvent* e)
{
	init_ui();

	QWidget::showEvent(e);
}

