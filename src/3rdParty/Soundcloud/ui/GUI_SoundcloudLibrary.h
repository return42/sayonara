/* GUI_SoundCloudLibrary.h */

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

#ifndef GUI_SOUNDCLOUDLIBRARY_H
#define GUI_SOUNDCLOUDLIBRARY_H

#include "GUI/Library/GUI_AbstractLibrary.h"
#include "Helper/Pimpl.h"

#include "3rdParty/Soundcloud/ui/GUI_SoundcloudArtistSearch.h"

#include <QtGlobal>

class QFrame;

namespace Ui
{
	class GUI_SoundcloudLibrary;
}

namespace SC
{
	class Library;
	class GUI_ArtistSearch;

	class GUI_Library :
			public GUI_AbstractLibrary
	{
		Q_OBJECT
		PIMPL(GUI_Library)

	public:
		explicit GUI_Library(SC::Library* library, QWidget *parent=nullptr);
		~GUI_Library();

		QMenu*		get_menu() const;
		QFrame*		header_frame() const;

	protected:
		::Library::TrackDeletionMode show_delete_dialog(int n_tracks) override;
		void init_shortcuts() override;

		LibraryTableView* lv_artist() const override;
		LibraryTableView* lv_album() const override;
		LibraryTableView* lv_tracks() const override;
		QPushButton* btn_clear() const override;
		QLineEdit* le_search() const override;
		QComboBox* combo_search() const override;

	protected slots:
		void btn_add_clicked();

	private:
		Ui::GUI_SoundcloudLibrary*	ui=nullptr;

	};
}
#endif // GUI_SOUNDCLOUDLIBRARY_H
