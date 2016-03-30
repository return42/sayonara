
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



#ifndef GUI_SHORTCUTENTRY_H
#define GUI_SHORTCUTENTRY_H

#include "GUI/Helper/SayonaraWidget.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"
#include "GUI/Shortcuts/ui_GUI_ShortcutEntry.h"

class GUI_ShortcutEntry :
		public SayonaraWidget,
		private Ui::GUI_ShortcutEntry
{
	Q_OBJECT

signals:
	void sig_test_pressed(const QList<QKeySequence>& sequences);

public:
	GUI_ShortcutEntry(const Shortcut& shortcut, QWidget* parent=nullptr);

public slots:
	void commit();
	void clear();
	void revert();


private slots:
	void edit_clicked();
	void default_clicked();
	void test_clicked();



private:
	ShortcutHandler*		_sch=nullptr;
	QString					_key;
	Shortcut				_shortcut;
};


#endif // GUI_SHORTCUTENTRY_H
