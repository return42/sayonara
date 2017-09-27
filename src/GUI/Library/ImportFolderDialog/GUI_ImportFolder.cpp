/* GUIImportFolder.cpp */

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

#include "GUI_ImportFolder.h"
#include "GUI/Library/ui_GUI_ImportFolder.h"
#include "GUI/TagEdit/GUI_TagEdit.h"

#include "Components/TagEdit/TagEdit.h"
#include "Components/Library/LocalLibrary.h"

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Message/Message.h"
#include "Helper/Language.h"

#include <QPixmap>
#include <QScrollBar>
#include <QFileDialog>
#include <QCloseEvent>
#include <QShowEvent>


struct GUI_ImportFolder::Private
{
	LibraryImporter*	importer=nullptr;
	GUI_TagEdit*		tag_edit=nullptr;
	LocalLibrary*		library=nullptr;
};

GUI_ImportFolder::GUI_ImportFolder(LocalLibrary* library, bool copy_enabled, QWidget* parent) :
	SayonaraDialog(parent)
{
	m = Pimpl::make<Private>();
	ui = new Ui::ImportFolder();
	ui->setupUi(this);

	m->library = library;
	m->tag_edit = new GUI_TagEdit(this);
	m->tag_edit->hide();
	m->importer = library->importer();

	ui->lab_target_path->setText( library->library_path() );
	ui->lab_target_path->setVisible(copy_enabled);
	ui->lab_target_info->setVisible(copy_enabled);
	ui->pb_progress->setValue(0);
	ui->pb_progress->setVisible(false);
	//ui->btn_edit->setVisible(false);

	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_ImportFolder::bb_accepted);
	connect(ui->btn_choose_dir, &QPushButton::clicked, this, &GUI_ImportFolder::choose_dir);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_ImportFolder::bb_rejected);
	connect(ui->btn_edit, &QPushButton::clicked, this, &GUI_ImportFolder::edit_pressed);

	connect(m->importer, &LibraryImporter::sig_got_metadata, this, &GUI_ImportFolder::set_metadata);
	connect(m->importer, &LibraryImporter::sig_status_changed, this, &GUI_ImportFolder::set_status);
	connect(m->importer, &LibraryImporter::sig_progress, this, &GUI_ImportFolder::set_progress);
	connect(m->importer, &LibraryImporter::sig_triggered, this, &GUI_ImportFolder::show);

	setModal(true);
}

GUI_ImportFolder::~GUI_ImportFolder() {}

void GUI_ImportFolder::language_changed()
{
	ui->retranslateUi(this);
	ui->btn_edit->setText(Lang::get(Lang::Edit));
	ui->btn_cancel->setText(Lang::get(Lang::Cancel));
}

void GUI_ImportFolder::set_metadata(const MetaDataList& v_md)
{
	if(!v_md.isEmpty()){
		ui->lab_status->setText(tr("%1 tracks available").arg(v_md.size()));
	}

	m->tag_edit->get_tag_edit()->set_metadata(v_md);
	ui->btn_edit->setVisible( !v_md.isEmpty() );
}

void GUI_ImportFolder::set_status(LibraryImporter::ImportStatus status)
{
	ui->pb_progress->hide();
	ui->lab_status->show();

	bool thread_active = false;

	switch(status){
		case LibraryImporter::ImportStatus::Caching:
			ui->lab_status->setText(tr("Loading tracks") + "...");
			thread_active = true;
			show();
			break;

		case LibraryImporter::ImportStatus::Importing:
			ui->lab_status->setText(tr("Importing") + "...");
			thread_active = true;
			break;

		case LibraryImporter::ImportStatus::Imported:
			ui->lab_status->setText(tr("Finished"));
			close();
			break;

		case LibraryImporter::ImportStatus::Cancelled:
			ui->lab_status->setText(tr("Cancelled"));
			close();
			break;

		case LibraryImporter::ImportStatus::NoTracks:
			ui->lab_status->setText(tr("No tracks"));
			break;

		case LibraryImporter::ImportStatus::Rollback:
			ui->lab_status->setText(tr("Rollback"));
			thread_active = true;
			break;

		default:
			break;
	}

	if(thread_active){
		ui->btn_cancel->setText(tr("Cancel"));
	}

	else{
		ui->btn_cancel->setText(tr("Close"));
	}
}

void GUI_ImportFolder::set_progress(int val)
{
	if(val) {
		ui->pb_progress->show();
		ui->lab_status->hide();
	}

	else{
		ui->pb_progress->hide();
	}

	ui->pb_progress->setValue(val);
	if(val == 100) {
		val = 0;
	}

	emit sig_progress(val);
}

void GUI_ImportFolder::bb_accepted()
{
	m->tag_edit->commit();

	QString target_dir = ui->le_directory->text();

	m->importer->accept_import(target_dir);
}

void GUI_ImportFolder::bb_rejected()
{
	//m->tag_edit->cancel();
	LibraryImporter::ImportStatus status = m->importer->get_status();

	m->importer->cancel_import();

	if( status == LibraryImporter::ImportStatus::Cancelled  ||
		status == LibraryImporter::ImportStatus::NoTracks ||
		status == LibraryImporter::ImportStatus::Imported)
	{
		close();
	}
}


void GUI_ImportFolder::choose_dir()
{
	QString library_path = m->library->library_path();
	QString dialog_title = tr("Choose target directory");
	QString dir =
	QFileDialog::getExistingDirectory(	this,
										dialog_title,
										library_path,
										QFileDialog::ShowDirsOnly
	);

	if(dir.isEmpty()){
		ui->le_directory->clear();
		return;
	}

	if(!dir.contains(library_path)) {
		Message::warning(tr("%1<br />is no library directory").arg(dir));
		ui->le_directory->clear();
		return;
	}

	dir.replace(library_path, "");
	while(dir.startsWith(QDir::separator())){
		dir.remove(0, 1);
	}

	while(dir.endsWith(QDir::separator())){
		dir.remove( dir.size() - 1, 1);
	}

	ui->le_directory->setText(dir);
}

void GUI_ImportFolder::edit_pressed()
{
	SayonaraDialog* dialog = m->tag_edit->box_into_dialog();

	connect(m->tag_edit, &GUI_TagEdit::sig_cancelled, dialog, &SayonaraDialog::reject);
	connect(m->tag_edit, &GUI_TagEdit::sig_ok_clicked, dialog, &SayonaraDialog::accept);

	m->tag_edit->show();
	dialog->exec();
}

void GUI_ImportFolder::closeEvent(QCloseEvent* e)
{
	SayonaraDialog::closeEvent(e);
	m->importer->cancel_import();
}

void GUI_ImportFolder::showEvent(QShowEvent* e)
{
	QDialog::showEvent(e);
	ui->lab_target_path->setText( m->library->library_path() );
}


