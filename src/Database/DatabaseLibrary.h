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

class MetaDataList;
namespace DB
{
	class Library :
			private Module
	{
		PIMPL(Library)

		public:
			Library(const QSqlDatabase& db, uint8_t db_id, int8_t library_id);
			~Library();

			virtual void drop_indexes();
			virtual void create_indexes();
			virtual bool store_metadata(const MetaDataList& v_md);
			virtual void add_album_artists();
	};
}

#endif // DATABASELIBRARY_H
