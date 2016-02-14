/* GUIImportFolder.cpp */

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

#include "GUI_ImportFolder.h"
#include "GUI/TagEdit/GUI_TagEdit.h"

#include "Helper/Helper.h"

#include <QPixmap>
#include <QScrollBar>
#include <QFileDialog>


GUI_ImportFolder::GUI_ImportFolder(QWidget* parent, bool copy_enabled) :
	SayonaraDialog(parent),
	Ui::ImportFolder()
{
	setupUi(this);

	_ui_tag_edit = new GUI_TagEdit(this);
	_importer = LibraryImporter::getInstance();

	tw_tabs->addTab(_ui_tag_edit, tr("Edit"));

	combo_folders->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);
	combo_folders->setVisible(copy_enabled);

	lab_target_path->setVisible(copy_enabled);
	lab_target_info->setVisible(copy_enabled);

	QString libpath = _settings->get(Set::Lib_Path);
	lab_target_path->setText( libpath );

	connect(btn_ok, &QPushButton::clicked, this, &GUI_ImportFolder::bb_accepted);
	connect(combo_folders, &QComboBox::editTextChanged, this, &GUI_ImportFolder::combo_box_changed);
	connect(btn_choose_dir, &QPushButton::clicked, this, &GUI_ImportFolder::choose_dir);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_ImportFolder::bb_rejected);

	connect(_importer, &LibraryImporter::sig_got_metadata, this, &GUI_ImportFolder::set_metadata);
	connect(_importer, &LibraryImporter::sig_status_changed, this, &GUI_ImportFolder::set_status);
	connect(_importer, &LibraryImporter::sig_progress, this, &GUI_ImportFolder::set_progress);
	connect(_importer, &LibraryImporter::sig_triggered, this, &GUI_ImportFolder::show);

	pb_progress->setValue(0);
	pb_progress->setVisible(false);

	setModal(true);
}


GUI_ImportFolder::~GUI_ImportFolder() {

}

void GUI_ImportFolder::language_changed(){
	retranslateUi(this);
}


void GUI_ImportFolder::set_import_dir(const QString& dir){
	_importer->import_dir(dir);
}

void GUI_ImportFolder::set_import_files(const QStringList& filelist){
	_importer->import_files(filelist);
}


void GUI_ImportFolder::set_folderlist(const QStringList& lst) {
	combo_folders->clear();
	combo_folders->addItems(lst);
}

void GUI_ImportFolder::set_metadata(const MetaDataList& v_md){

	_ui_tag_edit->get_tag_edit()->set_metadata(v_md);

	if(!v_md.isEmpty()){
		lab_status->setText(tr("%1 tracks available").arg(v_md.size()));
		tw_tabs->setTabEnabled(1, true);
	}

}

void GUI_ImportFolder::set_status(LibraryImporter::ImportStatus status) {

	pb_progress->hide();
	lab_status->show();

	tw_tabs->setCurrentIndex(0);
	tw_tabs->setTabEnabled(1, false );

	bool thread_active = false;

	switch(status){
		case LibraryImporter::ImportStatus::Caching:
			lab_status->setText(tr("Loading tracks..."));
			thread_active = true;
			show();
			break;

		case LibraryImporter::ImportStatus::Importing:
			lab_status->setText(tr("Importing..."));
			thread_active = true;
			break;

		case LibraryImporter::ImportStatus::Imported:
			lab_status->setText(tr("Done"));
			close();
			break;

		case LibraryImporter::ImportStatus::Cancelled:
			lab_status->setText(tr("Cancelled"));
			close();
			break;

		case LibraryImporter::ImportStatus::NoTracks:
			lab_status->setText(tr("No tracks"));
			break;

		case LibraryImporter::ImportStatus::Rollback:
			lab_status->setText(tr("Rollback"));
			thread_active = true;
			break;

		default:
			break;
	}

	if(thread_active){
		btn_cancel->setText(tr("Cancel"));
	}

	else{
		btn_cancel->setText(tr("Close"));
	}
}

void GUI_ImportFolder::set_progress(int val) {

	if(val) {
		pb_progress->show();
		lab_status->hide();
	}

	else{
		pb_progress->hide();
	}

	pb_progress->setValue(val);
	if(val == 100) {
		val = 0;
	}

	emit sig_progress(val);
}

void GUI_ImportFolder::bb_accepted() {

	QString target_dir = combo_folders->currentText().trimmed();

	_importer->accept_import(target_dir);
}

void GUI_ImportFolder::bb_rejected() {

	LibraryImporter::ImportStatus status = _importer->get_status();

	_importer->cancel_import();

	if( status == LibraryImporter::ImportStatus::Cancelled  ||
		status == LibraryImporter::ImportStatus::NoTracks ||
		status == LibraryImporter::ImportStatus::Imported)
	{
		close();
	}
}


void GUI_ImportFolder::choose_dir() {


	QString lib_path = _settings->get(Set::Lib_Path);
	QString dialog_title = tr("Choose target directory");
	QString dir =
		QFileDialog::getExistingDirectory(	this,
											dialog_title,
											lib_path,
											QFileDialog::ShowDirsOnly
	);

	if(dir.isEmpty()){
		return;
	}

	if(!dir.contains(lib_path)) {
		Message::warning(tr("%1<br />is no library directory").arg(dir));
		return;
	}

	QString parent_folder = FileHelper::get_parent_directory(dir);
	dir.remove(parent_folder);

	while(dir.startsWith(QDir::separator())) {
		dir=dir.remove(0, 1);
	}

	while(dir.endsWith(QDir::separator())) {
		dir = dir.remove(dir.size() - 1, 1);
	}

	combo_folders->setEditText(dir);
}


void GUI_ImportFolder::combo_box_changed(const QString& text) {

	QString lib_path = _settings->get(Set::Lib_Path);
	lab_target_path->setText( lib_path + QDir::separator() + text );
}



void GUI_ImportFolder::closeEvent(QCloseEvent* e) {
	QDialog::closeEvent(e);
	_importer->cancel_import();
}

void GUI_ImportFolder::showEvent(QShowEvent* e) {
	QDialog::showEvent(e);
}


