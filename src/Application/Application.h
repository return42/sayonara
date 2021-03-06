/* application.h */

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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "Utils/Pimpl.h"

class QTranslator;
class QStringList;

class Application :
		public QApplication
{
    Q_OBJECT
	PIMPL(Application)

public:
	Application(int& argc, char** argv);
	~Application();

	bool init(const QStringList& files_to_play);

	bool settings_initialized() const;

private:
	void init_single_instance_thread();
	void init_translator();
	void init_preferences();
	void init_libraries();
	void init_engine();
	void init_plugins();
	void init_player(QTranslator* translator);
	void init_playlist(const QStringList& files_to_play);
};

#endif // APPLICATION_H


