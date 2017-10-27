/* DatabaseConnector.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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

#ifndef DatabaseConnector_H
#define DatabaseConnector_H

#include "Database/AbstractDatabase.h"
#include "Database/DatabaseBookmarks.h"
#include "Database/DatabaseModule.h"
#include "Database/DatabasePlaylist.h"
#include "Database/DatabasePodcasts.h"
#include "Database/DatabaseSettings.h"
#include "Database/DatabaseStreams.h"
#include "Database/DatabaseVisStyles.h"
#include "Database/LibraryDatabase.h"

#include "Utils/Singleton.h"

#include <QList>

#define INDEX_SIZE 3

class LibraryDatabase;
class LocalLibraryDatabase;
namespace DB
{
	class Connector :
			public Base,
			public Bookmarks,
			public Playlist,
			public Podcasts,
			public Settings,
			public Streams,
			public VisualStyles
	{
		SINGLETON(Connector)

		private:
			QList<LibraryDatabase*> _library_dbs;
			LocalLibraryDatabase*	_local_library=nullptr;


		protected:
			bool updateAlbumCissearchFix();
			bool updateArtistCissearchFix();
			bool updateTrackCissearchFix();

			virtual bool apply_fixes();

		public:
			virtual void clean_up();
			void register_library_db(int8_t library_id);
			QList<LibraryDatabase*> library_dbs() const;
			LibraryDatabase* library_db(int8_t library_id, uint8_t db_id);

			template<typename T>
			LibraryDatabase* register_library_db(int8_t library_id)
			{
				for(int i=0; i<_library_dbs.size(); i++)
				{
					LibraryDatabase* lib_db = _library_dbs[i];
					if(lib_db->library_id() == library_id &&
					   lib_db->db_id() == db_id())
					{
						return lib_db;
					}
				}

				DB::LibraryDatabase* lib_db = new T(library_id);
				_library_dbs << lib_db;
				return lib_db;
			}
	};
}
#endif // DatabaseConnector_H
