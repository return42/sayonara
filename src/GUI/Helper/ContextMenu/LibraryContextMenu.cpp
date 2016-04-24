/* LibraryContextMenu.cpp */

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



#include "LibraryContextMenu.h"

#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GUI_Helper.h"

LibraryContextMenu::LibraryContextMenu(QWidget* parent) :
	QMenu(parent),
	SayonaraClass()
{
	_info_action = new QAction(GUI::get_icon("info"), tr("Info"), this);
	_lyrics_action  = new QAction(GUI::get_icon("lyrics"), tr("Lyrics"), this);
	_edit_action = new QAction(GUI::get_icon("edit"), tr("Edit"), this);
	_remove_action = new QAction(GUI::get_icon("delete"), tr("Remove"), this);
	_delete_action = new QAction(GUI::get_icon("delete"), tr("Delete"), this);
	_play_next_action = new QAction(GUI::get_icon("fwd_orange"), tr("Play next"), this);
	_append_action = new QAction(GUI::get_icon("append"), tr("Append"), this);
	_refresh_action = new QAction(GUI::get_icon("undo"), tr("Refresh"), this);
	_clear_action = new QAction(GUI::get_icon("broom.png"), tr("Clear"), this);

	connect(_info_action, &QAction::triggered, this, &LibraryContextMenu::sig_info_clicked);
	connect(_lyrics_action, &QAction::triggered, this, &LibraryContextMenu::sig_lyrics_clicked);
	connect(_edit_action, &QAction::triggered, this, &LibraryContextMenu::sig_edit_clicked);
	connect(_remove_action, &QAction::triggered, this, &LibraryContextMenu::sig_remove_clicked);
	connect(_delete_action, &QAction::triggered, this, &LibraryContextMenu::sig_delete_clicked);
	connect(_play_next_action, &QAction::triggered, this, &LibraryContextMenu::sig_play_next_clicked);
	connect(_append_action, &QAction::triggered, this, &LibraryContextMenu::sig_append_clicked);
	connect(_refresh_action, &QAction::triggered, this, &LibraryContextMenu::sig_refresh_clicked);
	connect(_clear_action, &QAction::triggered, this, &LibraryContextMenu::sig_clear_clicked);

	QList<QAction*> actions;
	actions << _info_action
			<< _lyrics_action
			<< _edit_action
			<< _remove_action
			<< _delete_action
			<< _play_next_action
			<< _append_action
			<< _refresh_action
			<< _clear_action;

	this->addActions(actions);

	for(QAction* action : actions){
		action->setVisible(false);
	}

	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}


LibraryContextMenu::~LibraryContextMenu() {

	delete _info_action;
	delete _lyrics_action;
	delete _edit_action;
	delete _remove_action;
	delete _delete_action;
	delete _play_next_action;
	delete _refresh_action;
	delete _clear_action;
}


void LibraryContextMenu::changeEvent(QEvent* e) {
	if (e->type() == QEvent::LanguageChange) {
		_info_action->setText(tr("Info"));
		_lyrics_action->setText(tr("Lyrics"));
		_edit_action->setText(tr("Edit"));
		_remove_action->setText(tr("Remove"));
		_delete_action->setText(tr("Delete"));
		_play_next_action->setText(tr("Play next"));
		_append_action->setText(tr("Append"));
		_refresh_action->setText(tr("Refresh"));
		_clear_action->setText(tr("Clear"));

		return;
	}

	QMenu::changeEvent(e);
}

LibraryContexMenuEntries LibraryContextMenu::get_entries() const
{
	LibraryContexMenuEntries entries = EntryNone;

	if(_info_action->isVisible()){
		entries |= EntryInfo;
	}

	if(_lyrics_action->isVisible()){
		entries |= EntryDelete;
	}
	if(_edit_action->isVisible()){
		entries |= EntryEdit;
	}
	if(_remove_action->isVisible()){
		entries |= EntryRemove;
	}
	if(_delete_action->isVisible()){
		entries |= EntryDelete;
	}
	if(_play_next_action->isVisible()){
		entries |= EntryPlayNext;
	}
	if(_append_action->isVisible()){
		entries |= EntryAppend;
	}
	if(_refresh_action->isVisible()){
		entries |= EntryRefresh;
	}
	if(_clear_action->isVisible()){
		entries |= EntryClear;
	}

	return entries;
}


void LibraryContextMenu::show_actions(LibraryContexMenuEntries entries) {

	_info_action->setVisible(entries & EntryInfo);
	_lyrics_action->setVisible(entries & EntryLyrics);
	_edit_action->setVisible(entries & EntryEdit);
	_remove_action->setVisible(entries & EntryRemove);
	_delete_action->setVisible(entries & EntryDelete);
	_play_next_action->setVisible(entries & EntryPlayNext);
	_append_action->setVisible(entries & EntryAppend);
	_refresh_action->setVisible(entries & EntryRefresh);
	_clear_action->setVisible(entries & EntryClear);

}

void LibraryContextMenu::show_action(LibraryContextMenu::Entry entry, bool visible){

	LibraryContexMenuEntries entries = this->get_entries();
	if(visible){
		entries |= entry;
	}

	else{
		entries &= ~(entry);
	}

	show_actions(entries);
}

void LibraryContextMenu::show_all(){
	for(QAction* action : this->actions()){
		action->setVisible(true);
	}
}


void LibraryContextMenu::skin_changed()
{
	IconLoader* icon_loader = IconLoader::getInstance();
	_info_action->setIcon(icon_loader->get_icon("dialog-information", "info"));
	_lyrics_action->setIcon(icon_loader->get_icon("document-properties", "lyrics"));
	_edit_action->setIcon(icon_loader->get_icon("accessories-text-editor", "edit"));
	_remove_action->setIcon(icon_loader->get_icon("list-remove", "delete"));
	_delete_action->setIcon(icon_loader->get_icon("edit-delete", "delete"));
	_play_next_action->setIcon(icon_loader->get_icon("media-playback-start", "play"));
	_append_action->setIcon(icon_loader->get_icon("list-add", "append"));
	_refresh_action->setIcon(icon_loader->get_icon("edit-undo", "undo"));
	_clear_action->setIcon(icon_loader->get_icon("edit-clear", "broom"));
}
