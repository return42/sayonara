/* GUI_SoundCloudLibrary.cpp */

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

#include "3rdParty/Soundcloud/ui/GUI_SoundcloudLibrary.h"
#include "3rdParty/Soundcloud/ui_GUI_SoundcloudLibrary.h"

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"

#include <QShortcut>

struct GUI_SoundCloudLibrary::Private
{
	GUI_SoundcloudArtistSearch* artist_search=nullptr;
	QMenu*						library_menu=nullptr;
};

GUI_SoundCloudLibrary::GUI_SoundCloudLibrary(SoundcloudLibrary* library, QWidget *parent) :
	GUI_AbstractLibrary(library, parent)
{
	setup_parent(this, &ui);

	_m = Pimpl::make<GUI_SoundCloudLibrary::Private>();
	_m->artist_search = new GUI_SoundcloudArtistSearch(library, this);
	_m->library_menu = new QMenu(this);

	QAction* action_add_artist = _m->library_menu->addAction(tr("Add artist"));

	setAcceptDrops(false);

	connect(action_add_artist, &QAction::triggered, this, &GUI_SoundCloudLibrary::btn_add_clicked);

	LibraryContexMenuEntries entry_mask =
			(LibraryContextMenu::EntryPlayNext |
			LibraryContextMenu::EntryInfo |
			LibraryContextMenu::EntryDelete |
			LibraryContextMenu::EntryAppend |
			LibraryContextMenu::EntryRefresh);

	ui->tb_title->show_rc_menu_actions(entry_mask);
	ui->lv_album->show_rc_menu_actions(entry_mask);
	ui->lv_artist->show_rc_menu_actions(entry_mask);

	library->load();
}

GUI_SoundCloudLibrary::~GUI_SoundCloudLibrary()
{
	if(ui)
	{
		delete ui; ui = nullptr;
	}
}


QComboBox* GUI_SoundCloudLibrary::get_libchooser() const
{
	return ui->combo_lib_chooser;
}


QMenu* GUI_SoundCloudLibrary::get_menu() const
{
	return _m->library_menu;
}

Library::TrackDeletionMode GUI_SoundCloudLibrary::show_delete_dialog(int n_tracks){
	Q_UNUSED(n_tracks)
	return Library::TrackDeletionMode::OnlyLibrary;
}

void GUI_SoundCloudLibrary::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), ui->le_search, SLOT(setFocus()), nullptr, Qt::WidgetWithChildrenShortcut);
	new QShortcut(QKeySequence("Esc"), this, SLOT(clear_button_pressed()), nullptr, Qt::WidgetWithChildrenShortcut);
}

void GUI_SoundCloudLibrary::btn_add_clicked()
{
	_m->artist_search->show();
}

