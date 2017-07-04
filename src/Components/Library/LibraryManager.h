/* LibraryManager.h */

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



#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QList>

class LibraryInfo;
class LocalLibrary;
class LocalLibraryContainer;

class LibraryManager :
		public SayonaraClass
{
    PIMPL(LibraryManager)
    SINGLETON(LibraryManager)

	friend class LocalLibrary;

public:
	int8_t add_library(const QString& name, const QString& path);
	void rename_library(int8_t id, const QString& name);
	void remove_library(int8_t id);
	void move_library(int old_row, int new_row);
	void change_library_path(int8_t id, const QString& path);

	static QString request_library_name(const QString& path);
	QList<LibraryInfo> all_libraries() const;
	LibraryInfo library_info(int8_t id) const;

    int count() const;

	LocalLibrary* library_instance(int8_t id) const;

	void revert();
};


#endif // LIBRARYMANAGER_H
