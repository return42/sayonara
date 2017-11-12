/* DatabaseLibrary.h */

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

#ifndef DATABASELIBRARY_H
#define DATABASELIBRARY_H

#include "Database/DatabaseModule.h"
#include "Utils/Pimpl.h"
#include <QMap>

class MetaDataList;

namespace Library
{
	class Info;
}

namespace DB
{
	class Library :
			private Module
	{
		PIMPL(Library)

		public:
			Library(const QSqlDatabase& db, uint8_t db_id);
			~Library();

			QList<::Library::Info> get_all_libraries();
			bool insert_library(int8_t library_id, const QString& library_name, const QString& library_path, int index);
			bool edit_library(int8_t library_id, const QString& new_name, const QString& new_path);
			bool remove_library(int8_t library_id);
			bool reorder_libraries(const QMap<int8_t, int>& order);

			virtual void drop_indexes();
			virtual void create_indexes();

			virtual void add_album_artists();
	};
}

#endif // DATABASELIBRARY_H
