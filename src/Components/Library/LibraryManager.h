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

#include "Utils/Singleton.h"
#include "Utils/Pimpl.h"
#include "Utils/Settings/SayonaraClass.h"

#include <QList>

class LocalLibrary;
class LocalLibraryContainer;

namespace Library
{
	class Info;
	class Manager :
			public SayonaraClass
	{
		PIMPL(Manager)
		SINGLETON(Manager)

		friend class LocalLibrary;

	public:
		LibraryId add_library(const QString& name, const QString& path);
		bool rename_library(LibraryId id, const QString& name);
		bool remove_library(LibraryId id);
		bool move_library(int old_row, int new_row);
		bool change_library_path(LibraryId id, const QString& path);

		static QString request_library_name(const QString& path);
		QList<Info> all_libraries() const;
		Info library_info(LibraryId id) const;

		int count() const;

		LocalLibrary* library_instance(LibraryId id) const;

		void revert();
	};
}


#endif // LIBRARYMANAGER_H
