
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

#include "TabMenu.h"

#include "GUI/Utils/IconLoader/IconLoader.h"
#include "GUI/Utils/GuiUtils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"

using namespace Gui;

struct PlaylistTabMenu::Private
{
	QAction*	action_open_file=nullptr;
	QAction*	action_open_dir=nullptr;
	QAction*	action_delete=nullptr;
	QAction*	action_save=nullptr;
	QAction*	action_save_as=nullptr;
	QAction*    action_save_to_file=nullptr;
	QAction*	action_reset=nullptr;
	QAction*	action_close=nullptr;
	QAction*	action_close_others=nullptr;
	QAction*	action_rename=nullptr;
	QAction*	action_clear=nullptr;
};

PlaylistTabMenu::PlaylistTabMenu(QWidget* parent) :
	WidgetTemplate<QMenu>(parent)
{
	m = Pimpl::make<Private>();

	m->action_open_file = new QAction(this);
	m->action_open_dir = new QAction(this);
	m->action_reset = new QAction(this);
	m->action_rename = new QAction(this);
	m->action_delete = new QAction(this);
	m->action_save = new QAction(this);
	m->action_save_as = new QAction(this);
	m->action_save_to_file = new QAction(this);
	m->action_clear = new QAction(this);
	m->action_close = new QAction(this);
	m->action_close_others = new QAction(this);


	QList<QAction*> actions;
	actions << m->action_open_file
			<< m->action_open_dir
			<< this->addSeparator()
			<< m->action_reset
			<< this->addSeparator()
			<< m->action_rename
			<< m->action_save
			<< m->action_save_as
			<< m->action_save_to_file
			<< m->action_delete
			<< this->addSeparator()
			<< m->action_clear
			<< this->addSeparator()
			<< m->action_close_others
			<< m->action_close;

	this->addActions(actions);

	connect(m->action_open_file, &QAction::triggered, this, &PlaylistTabMenu::sig_open_file_clicked);
	connect(m->action_open_dir, &QAction::triggered, this, &PlaylistTabMenu::sig_open_dir_clicked);
	connect(m->action_reset, &QAction::triggered, this, &PlaylistTabMenu::sig_reset_clicked);
	connect(m->action_rename, &QAction::triggered, this, &PlaylistTabMenu::sig_rename_clicked);
	connect(m->action_delete, &QAction::triggered, this, &PlaylistTabMenu::sig_delete_clicked);
	connect(m->action_save, &QAction::triggered, this, &PlaylistTabMenu::sig_save_clicked);
	connect(m->action_save_as, &QAction::triggered, this, &PlaylistTabMenu::sig_save_as_clicked);
	connect(m->action_save_to_file, &QAction::triggered, this, &PlaylistTabMenu::sig_save_to_file_clicked);
	connect(m->action_clear, &QAction::triggered, this, &PlaylistTabMenu::sig_clear_clicked);
	connect(m->action_close, &QAction::triggered, this, &PlaylistTabMenu::sig_close_clicked);
	connect(m->action_close_others, &QAction::triggered, this, &PlaylistTabMenu::sig_close_others_clicked);
}

PlaylistTabMenu::~PlaylistTabMenu()
{
	this->clear();
}

void PlaylistTabMenu::language_changed()
{
	m->action_open_file->setText(Lang::get(Lang::OpenFile).triplePt());
	m->action_open_dir->setText(Lang::get(Lang::OpenDir).triplePt());
	m->action_reset->setText(Lang::get(Lang::Reset));
	m->action_rename->setText(Lang::get(Lang::Rename).triplePt());
	m->action_delete->setText(Lang::get(Lang::Delete));
	m->action_save->setText(Lang::get(Lang::Save));
	m->action_save_as->setText(Lang::get(Lang::SaveAs).triplePt());
	m->action_save_to_file->setText(Lang::get(Lang::SaveToFile));
	m->action_clear->setText(Lang::get(Lang::Clear));
	m->action_close->setText(Lang::get(Lang::Close));
	m->action_close_others->setText(Lang::get(Lang::CloseOthers));
}

void PlaylistTabMenu::skin_changed()
{
	m->action_open_file->setIcon(IconLoader::icon( "document-open", "play_small") );
	m->action_open_dir->setIcon(IconLoader::icon( "document-open", "play_small") );

	m->action_reset->setIcon(IconLoader::icon( "edit-undo", "undo") );
	m->action_rename->setIcon(IconLoader::icon( "accessories-text-editor", "edit") );
	m->action_delete->setIcon(IconLoader::icon( "edit-delete", "delete") );
	m->action_save->setIcon(IconLoader::icon( "document-save", "save") );
	m->action_save_as->setIcon(IconLoader::icon( "document-save-as", "save_as") );
	m->action_save_to_file->setIcon(IconLoader::icon( "document-save-as", "save_as") );

	m->action_clear->setIcon(IconLoader::icon( "edit-clear", "broom.png") );
	m->action_close->setIcon(IconLoader::icon( "window-close", "power_off") );

	m->action_close_others->setIcon(IconLoader::icon( "window-close", "power_on") );
}

void PlaylistTabMenu::show_menu_items(PlaylistMenuEntries entries)
{
	m->action_open_file->setVisible(entries & PlaylistMenuEntry::OpenFile);
	m->action_open_dir->setVisible(entries & PlaylistMenuEntry::OpenDir);
	m->action_reset->setVisible(entries & PlaylistMenuEntry::Reset);
	m->action_rename->setVisible(entries & PlaylistMenuEntry::Rename);
	m->action_delete->setVisible(entries & PlaylistMenuEntry::Delete);
	m->action_save->setVisible(entries & PlaylistMenuEntry::Save);
	m->action_save_as->setVisible(entries & PlaylistMenuEntry::SaveAs);
	m->action_save_to_file->setVisible(entries & PlaylistMenuEntry::SaveToFile);
	m->action_clear->setVisible(entries & PlaylistMenuEntry::Clear);
	m->action_close->setVisible(entries & PlaylistMenuEntry::Close);
	m->action_close_others->setVisible(entries & PlaylistMenuEntry::CloseOthers);
}


void PlaylistTabMenu::show_close(bool b)
{
	m->action_close->setVisible(b);
	m->action_close_others->setVisible(b);
}

