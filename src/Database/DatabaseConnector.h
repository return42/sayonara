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

#include "Utils/Singleton.h"
#include "Utils/Pimpl.h"

#include <QList>

#define INDEX_SIZE 3

namespace DB
{
	class LibraryDatabase;
	class Bookmarks;
	class Playlist;
	class LibraryDatabase;
	class LocalLibraryDatabase;
	class Podcasts;
	class Streams;
	class VisualStyles;
	class Settings;
	class Library;

	using LibraryDatabases=QList<LibraryDatabase*>;

	class Connector :
			public Base
	{
		SINGLETON(Connector)
		PIMPL(Connector)

		protected:
			bool updateAlbumCissearchFix();
			bool updateArtistCissearchFix();
			bool updateTrackCissearchFix();

			virtual bool apply_fixes();

		public:
			virtual void			clean_up();

			LibraryDatabases		library_dbs() const;
			DB::LibraryDatabase*	library_db(LibraryId library_id, DbId db_id);
			DB::LibraryDatabase*	find_library_db(LibraryId library_id) const;
			DB::LibraryDatabase*	register_library_db(LibraryId library_id);

			DB::Bookmarks*			bookmark_connector();
			DB::Playlist*			playlist_connector();
			DB::Podcasts*			podcast_connector();
			DB::Streams*			stream_connector();
			DB::VisualStyles*		visual_style_connector();
			DB::Settings*			settings_connector();
			DB::Library*			library_connector();
	};
}
#endif // DatabaseConnector_H
