/* LocalLibraryMenu.h */

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



#ifndef LOCALLIBRARYMENU_H
#define LOCALLIBRARYMENU_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QTimer>
#include <QShowEvent>

#include "Helper/SayonaraClass.h"




class LocalLibraryMenu :
		public QMenu,
		private SayonaraClass
{
	Q_OBJECT

signals:
	void sig_reload_library();
	void sig_import_file();
	void sig_import_folder();
	void sig_info();


public:
	LocalLibraryMenu(QWidget* parent=nullptr);
	~LocalLibraryMenu();

private:
	QAction* _reload_library_action=nullptr;
	QAction* _import_file_action=nullptr;
	QAction* _import_folder_action=nullptr;
	QAction* _info_action=nullptr;
	QTimer* _timer=nullptr;

	QList<QAction*> _actions;

	void showEvent(QShowEvent* e) override;


private slots:
	void language_changed();
	void timed_out();

};

#endif // LOCALLIBRARYMENU_H
