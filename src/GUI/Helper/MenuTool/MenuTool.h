/* MenuTool.h */

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



#ifndef MENUTOOL_H
#define MENUTOOL_H

#include <QMenu>

#include <QMouseEvent>
#include <QList>
#include <QAction>
#include <QTimer>

#include "MenuButton.h"
#include "GUI/Helper/ContextMenu/ContextMenu.h"


class MenuToolButton : public MenuButton {

	Q_OBJECT

signals:
	void sig_open();
	void sig_new();
	void sig_undo();
	void sig_save();
	void sig_save_as();
	void sig_rename();
	void sig_delete();

public:
	MenuToolButton(QWidget *parent=nullptr);
	virtual ~MenuToolButton();

	/**
	 * @brief Use this to add custom actions
	 * @param action
	 */
	void register_action(QAction* action);


	ContextMenuEntries get_entries() const;

private:
	ContextMenu* _menu=nullptr;

	void show_menu(QPoint pos) override;
	bool prove_enabled() override;


public slots:
	void show_action(ContextMenu::Entry entry, bool visible);
	void show_actions(ContextMenuEntries options);
	void show_all();

};




#endif // MENUTOOL_H
