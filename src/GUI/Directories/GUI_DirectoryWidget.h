/* GUI_DirectoryWidget.h */

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

#ifndef GUI_DIRECTORYWIDGET_H
#define GUI_DIRECTORYWIDGET_H

#include "GUI/Utils/Widgets/Widget.h"
#include "GUI/InfoDialog/InfoDialogContainer.h"
#include "Utils/MetaData/MetaDataFwd.h"
#include "Utils/Pimpl.h"

class QFrame;
class QComboBox;

UI_FWD(GUI_DirectoryWidget)

class GUI_DirectoryWidget :
		public Gui::Widget,
		public InfoDialogContainer
{
	Q_OBJECT
	PIMPL(GUI_DirectoryWidget)
	UI_CLASS(GUI_DirectoryWidget)

	public:
		explicit GUI_DirectoryWidget(QWidget* parent=nullptr);
		~GUI_DirectoryWidget();

		QFrame* header_frame() const;

	private:
		void init_shortcuts();

	protected:
		void language_changed() override;

	private slots:
		void search_button_clicked();

		void dir_enter_pressed();
		void dir_opened(QModelIndex idx);
		void dir_pressed(QModelIndex idx);
		void dir_append_clicked();
		void dir_play_next_clicked();
		void dir_delete_clicked();

		void file_dbl_clicked(QModelIndex idx);
		void file_enter_pressed();
		void file_pressed(QModelIndex idx);
		void file_append_clicked();
		void file_play_next_clicked();
		void file_delete_clicked();

		void import_requested(LibraryId library_id, const QStringList& paths, const QString& target_dir);
		void import_dialog_requested(const QString& target_dir);

		protected:
		MD::Interpretation metadata_interpretation() const override;
		MetaDataList info_dialog_data() const override;


};

#endif // GUI_DIRECTORYWIDGET_H
