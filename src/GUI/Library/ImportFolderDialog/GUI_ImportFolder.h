/* GUIImportFolder.h */

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

#ifndef GUIIMPORTFOLDER_H_
#define GUIIMPORTFOLDER_H_

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Components/Library/Importer/LibraryImporter.h"
#include "Helper/Pimpl.h"

namespace Ui { class ImportFolder; }

class MetaDataList;
class GUI_TagEdit;
class LocalLibrary;
class GUI_ImportFolder :
		public SayonaraDialog
{
	Q_OBJECT
	PIMPL(GUI_ImportFolder)

signals:
	void sig_progress(int);

public:
	GUI_ImportFolder(const LocalLibrary* library, bool copy_enabled, QWidget* parent);
	virtual ~GUI_ImportFolder();

private slots:
	void bb_accepted();
	void bb_rejected();
	void choose_dir();
	void edit_pressed();
	void set_metadata(const MetaDataList& v_md);
	void set_status(LibraryImporter::ImportStatus status);
	void set_progress(int);

private:
	void closeEvent(QCloseEvent* e) override;
	void showEvent(QShowEvent* e) override;
	void language_changed() override;

private:
	Ui::ImportFolder*	ui=nullptr;
};

#endif /* GUIIMPORTFOLDER_H_ */
