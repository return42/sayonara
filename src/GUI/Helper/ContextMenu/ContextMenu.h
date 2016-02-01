/* ContextMenu.h */

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



#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QAction>
#include <QMenu>
#include <QTimer>

#include "Helper/SayonaraClass.h"

typedef int ContextMenuEntries;

class ContextMenu :
		public QMenu,
		protected SayonaraClass
{
	Q_OBJECT

public:
	enum Entry {
		EntryNone	=0,
		EntryNew	=(1<<0),
		EntryUndo	=(1<<1),
		EntrySave	=(1<<2),
		EntrySaveAs	=(1<<3),
		EntryRename	=(1<<4),
		EntryDelete	=(1<<5),
		EntryOpen	=(1<<6)
	};

signals:
	void sig_new();
	void sig_undo();
	void sig_save();
	void sig_save_as();
	void sig_rename();
	void sig_delete();
	void sig_open();


private:
	QAction*	_action_new=nullptr;
	QAction*	_action_open=nullptr;
	QAction*	_action_undo=nullptr;
	QAction*	_action_save=nullptr;
	QAction*	_action_save_as=nullptr;
	QAction*	_action_rename=nullptr;
	QAction*	_action_delete=nullptr;


	QList<QAction*>		_actions;
	QTimer*				_timer=nullptr;

	void show_action(bool b, QAction* action);


public:
	explicit ContextMenu(QWidget *parent=nullptr);
	void register_action(QAction* action);
	bool has_actions();
	ContextMenuEntries get_entries() const;


protected:
	void showEvent(QShowEvent* e) override;


public slots:
	void show_actions(ContextMenuEntries entries);
	void show_action(ContextMenu::Entry entry, bool visible);
	void show_all();


private slots:
	void timed_out();
	void language_changed();
};



#endif // CONTEXTMENU_H
