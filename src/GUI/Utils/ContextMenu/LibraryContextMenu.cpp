/* LibraryContextMenu.cpp */

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

#include "LibraryContextMenu.h"

#include "GUI/Utils/IconLoader/IconLoader.h"
#include "GUI/Utils/GuiUtils.h"
#include "GUI/Utils/RatingLabel/RatingLabel.h"

#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"

#include <QMap>

struct LibraryContextMenu::Private
{
    QMap<LibraryContextMenu::Entry, QAction*> entry_action_map;
};

LibraryContextMenu::LibraryContextMenu(QWidget* parent) :
	WidgetTemplate<QMenu>(parent)
{
    m = Pimpl::make<Private>();

    _info_action = new QAction(this);
    _lyrics_action  = new QAction(this);
    _edit_action = new QAction(this);
    _remove_action = new QAction(this);
    _delete_action = new QAction(this);
    _play_next_action = new QAction(this);
    _append_action = new QAction(this);
    _refresh_action = new QAction(this);
    _clear_action = new QAction(this);
    _clear_selection_action = new QAction(this);
    _cover_view_action = new QAction(this);

	QList<QAction*> rating_actions;
	for(int i=0; i<=5; i++)
	{
		rating_actions << init_rating_action(i);
	}

	_rating_menu = new QMenu(this);
	_rating_menu->addActions(rating_actions);

	connect(_info_action, &QAction::triggered, this, &LibraryContextMenu::sig_info_clicked);
	connect(_lyrics_action, &QAction::triggered, this, &LibraryContextMenu::sig_lyrics_clicked);
	connect(_edit_action, &QAction::triggered, this, &LibraryContextMenu::sig_edit_clicked);
	connect(_remove_action, &QAction::triggered, this, &LibraryContextMenu::sig_remove_clicked);
	connect(_delete_action, &QAction::triggered, this, &LibraryContextMenu::sig_delete_clicked);
	connect(_play_next_action, &QAction::triggered, this, &LibraryContextMenu::sig_play_next_clicked);
	connect(_append_action, &QAction::triggered, this, &LibraryContextMenu::sig_append_clicked);
	connect(_refresh_action, &QAction::triggered, this, &LibraryContextMenu::sig_refresh_clicked);
	connect(_clear_action, &QAction::triggered, this, &LibraryContextMenu::sig_clear_clicked);
    connect(_clear_selection_action, &QAction::triggered, this, &LibraryContextMenu::sig_clear_selection_clicked);
    connect(_cover_view_action, &QAction::triggered, this, &LibraryContextMenu::cover_view_action_triggered);


    QList<QAction*> actions;
    actions << _info_action
			<< _lyrics_action
			<< _edit_action
			<< _remove_action
			<< _delete_action
            << _cover_view_action
			<< addSeparator()
			<< _play_next_action
			<< _append_action
			<< _refresh_action
			<< addSeparator()
			<< _clear_action
            << _clear_selection_action
			   ;

    this->addActions(actions);

	_rating_action = this->insertMenu(_remove_action, _rating_menu);
	_rating_action->setIcon(Gui::Util::icon("star.png"));
	_rating_action->setText(Lang::get(Lang::Rating));

    m->entry_action_map[EntryInfo] = _info_action;
    m->entry_action_map[EntryEdit] = _edit_action;
    m->entry_action_map[EntryLyrics] = _lyrics_action;
    m->entry_action_map[EntryRemove] = _remove_action;
    m->entry_action_map[EntryDelete] = _delete_action;
    m->entry_action_map[EntryPlayNext] = _play_next_action;
    m->entry_action_map[EntryAppend] = _append_action;
    m->entry_action_map[EntryRefresh] = _refresh_action;
    m->entry_action_map[EntryClear] = _clear_action;
    m->entry_action_map[EntryRating] = _rating_action;
    m->entry_action_map[EntryClearSelection] = _clear_selection_action;
    m->entry_action_map[EntryCoverView] = _cover_view_action;

	for(QAction* action : actions){
		action->setVisible(action->isSeparator());
	}
}

LibraryContextMenu::~LibraryContextMenu() {}

void LibraryContextMenu::language_changed()
{
	_info_action->setText(Lang::get(Lang::Info));
	_lyrics_action->setText(Lang::get(Lang::Lyrics));
	_edit_action->setText(Lang::get(Lang::Edit));
	_remove_action->setText(Lang::get(Lang::Remove));
	_delete_action->setText(Lang::get(Lang::Delete));
	_play_next_action->setText(Lang::get(Lang::PlayNext));
	_append_action->setText(Lang::get(Lang::Append));
	_refresh_action->setText(Lang::get(Lang::Refresh));
	_clear_action->setText(Lang::get(Lang::Clear));
	_rating_action->setText(Lang::get(Lang::Rating));
    _clear_selection_action->setText(tr("Clear selection"));

    bool show_covers = _settings->get(Set::Lib_ShowAlbumCovers);

    if(show_covers)
    {
        _cover_view_action->setText(tr("Switch to table view"));
    }

    else
    {
        _cover_view_action->setText(tr("Switch to cover view"));
    }
}


void LibraryContextMenu::skin_changed()
{
    ;

    _info_action->setIcon(IconLoader::icon("dialog-information", "info"));
    _lyrics_action->setIcon(IconLoader::icon("document-properties", "lyrics"));
    _edit_action->setIcon(IconLoader::icon("accessories-text-editor", "edit"));
    _remove_action->setIcon(IconLoader::icon("list-remove", "delete"));
    _delete_action->setIcon(IconLoader::icon("edit-delete", "delete"));
    _play_next_action->setIcon(IconLoader::icon("media-playback-start", "play_small"));
    _append_action->setIcon(IconLoader::icon("list-add", "append"));
    _refresh_action->setIcon(IconLoader::icon("edit-undo", "undo"));
    _clear_action->setIcon(IconLoader::icon("edit-clear", "broom.png"));
    _rating_action->setIcon(Gui::Util::icon("star.png"));
    _clear_selection_action->setIcon(IconLoader::icon("edit-clear", "broom.png"));
}

LibraryContexMenuEntries LibraryContextMenu::get_entries() const
{
	LibraryContexMenuEntries entries = EntryNone;

    for(QAction* action : m->entry_action_map.values())
    {
        if(action->isVisible()){
            entries |= m->entry_action_map.key(action);
        }
    }

	return entries;
}


void LibraryContextMenu::show_actions(LibraryContexMenuEntries entries)
{
    for(QAction* action : m->entry_action_map.values())
    {
        action->setVisible( entries & m->entry_action_map.key(action) );
    }
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

void LibraryContextMenu::show_all()
{
	for(QAction* action : this->actions()){
		action->setVisible(true);
	}
}


QAction* LibraryContextMenu::init_rating_action(int rating)
{
	QAction* action = new QAction(QString::number(rating), nullptr);
	action->setData(rating);
	action->setCheckable(true);

	connect(action, &QAction::triggered, [=]()
{
		emit sig_rating_changed(rating);
	});

	return action;
}

void LibraryContextMenu::set_rating(int rating)
{
	QList<QAction*> actions = _rating_menu->actions();
	for(QAction* action : actions){
		int data = action->data().toInt();
		action->setChecked(data == rating);
	}

	QString rating_text = Lang::get(Lang::Rating);
	if(rating > 0){
		_rating_action->setText(rating_text + " (" + QString::number(rating) + ")");
	}

	else{
		_rating_action->setText(rating_text);
    }
}

QAction* LibraryContextMenu::get_action(LibraryContextMenu::Entry entry) const
{
    return m->entry_action_map[entry];
}

void LibraryContextMenu::show_covers_changed()
{
    bool show_covers = _settings->get(Set::Lib_ShowAlbumCovers);

    if(show_covers)
    {
        _cover_view_action->setText(tr("Switch to table view"));
    }

    else
    {
        _cover_view_action->setText(tr("Switch to cover view"));
    }
}

void LibraryContextMenu::cover_view_action_triggered()
{
    bool show_covers = _settings->get(Set::Lib_ShowAlbumCovers);
    _settings->set(Set::Lib_ShowAlbumCovers, !show_covers);
}

