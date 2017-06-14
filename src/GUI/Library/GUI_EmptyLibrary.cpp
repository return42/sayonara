/* GUI_EmptyLibrary.cpp */

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



#include "GUI_EmptyLibrary.h"
#include "GUI/Library/ui_GUI_EmptyLibrary.h"
#include "Helper/Language.h"
#include "Helper/Message/Message.h"
#include "Helper/Library/LibraryNamespaces.h"
#include "Components/Library/LibraryManager.h"
#include "Components/Library/LocalLibrary.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"

#include <QDir>
#include <QFileDialog>

struct GUI_EmptyLibrary::Private
{

};

GUI_EmptyLibrary::GUI_EmptyLibrary(QWidget* parent) :
    QWidget(parent)
{
	ui = new Ui::GUI_EmptyLibrary();
	ui->setupUi(this);

	ui->pb_progress->setVisible(false);

	connect(ui->btn_setLibrary, &QPushButton::clicked,
	        this, &GUI_EmptyLibrary::set_lib_path_clicked);
}

GUI_EmptyLibrary::~GUI_EmptyLibrary() {}

QFrame* GUI_EmptyLibrary::header_frame() const
{
	return ui->header_frame;
}

void GUI_EmptyLibrary::set_lib_path_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this,
	                                                Lang::get(Lang::OpenDir),
	                                                QDir::homePath(),
	                                                QFileDialog::ShowDirsOnly);
	if(dir.isEmpty()){
		return;
	}

	GlobalMessage::Answer answer = Message::question_yn(tr("Do you want to reload the Library?"), "Library");

	if(answer == GlobalMessage::Answer::No){
		return;
	}

	LibraryManager* lib_manager = LibraryManager::getInstance();
	QString name = LibraryManager::request_library_name(dir);

	qint8 id = lib_manager->add_library(name, dir);
	LocalLibrary* library = lib_manager->get_library_instance(id);

	library->psl_reload_library(false, Library::ReloadQuality::Accurate);

	//LibraryPluginHandler::getInstance()->set_current_index(0);
}
