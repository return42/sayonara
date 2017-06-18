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
#include "3rdParty/Soundcloud/src/SoundcloudLibrary.h"

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"

#include <QShortcut>

struct SC::GUI_Library::Private
{
	SC::GUI_ArtistSearch*	artist_search=nullptr;
	QMenu*					library_menu=nullptr;
};

SC::GUI_Library::GUI_Library(SC::Library* library, QWidget *parent) :
	GUI_AbstractLibrary(library, parent)
{
	setup_parent(this, &ui);
	setAcceptDrops(false);

	_m = Pimpl::make<SC::GUI_Library::Private>();

	_m->artist_search = new SC::GUI_ArtistSearch(library, this);
	_m->library_menu = new QMenu(this);

	/* Remove search options */
	while(ui->combo_searchfilter->count() > 1){
		int last = ui->combo_searchfilter->count() - 1;
		ui->combo_searchfilter->removeItem(last);
	}

	QAction* action_add_artist = _m->library_menu->addAction(tr("Add artist"));
	connect(action_add_artist, &QAction::triggered, this, &SC::GUI_Library::btn_add_clicked);

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

SC::GUI_Library::~GUI_Library()
{
	if(ui)
	{
		delete ui; ui = nullptr;
	}
}


QMenu* SC::GUI_Library::get_menu() const
{
	return _m->library_menu;
}

QFrame* SC::GUI_Library::header_frame() const
{
	return ui->header_frame;
}

Library::TrackDeletionMode SC::GUI_Library::show_delete_dialog(int n_tracks)
{
	Q_UNUSED(n_tracks)
	return ::Library::TrackDeletionMode::OnlyLibrary;
}

void SC::GUI_Library::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), ui->le_search, SLOT(setFocus()), nullptr, Qt::WidgetWithChildrenShortcut);
	new QShortcut(QKeySequence("Esc"), this, SLOT(clear_button_pressed()), nullptr, Qt::WidgetWithChildrenShortcut);
}

void SC::GUI_Library::btn_add_clicked()
{
	_m->artist_search->show();
}

LibraryTableView*SC::GUI_Library::lv_artist() const
{
	return ui->lv_artist;
}

LibraryTableView*SC::GUI_Library::lv_album() const
{
	return ui->lv_album;
}

LibraryTableView*SC::GUI_Library::lv_tracks() const
{
	return ui->tb_title;
}

QPushButton*SC::GUI_Library::btn_clear() const
{
	return ui->btn_clear;
}

QLineEdit*SC::GUI_Library::le_search() const
{
	return ui->le_search;
}

QComboBox*SC::GUI_Library::combo_search() const
{
	return ui->combo_searchfilter;
}
