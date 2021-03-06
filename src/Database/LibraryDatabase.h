/* LibraryDatabase.h */

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

#ifndef LIBRARYDATABASE_H
#define LIBRARYDATABASE_H

#include "Database/AbstractDatabase.h"
#include "Database/DatabaseAlbums.h"
#include "Database/DatabaseArtists.h"
#include "Database/DatabaseTracks.h"
#include "Database/DatabaseLibrary.h"
#include "Utils/Pimpl.h"

namespace DB
{
	class LibraryDatabase :
			public DB::Base,
			public DB::Albums,
			public DB::Artists,
			public DB::Tracks
	{
		PIMPL(LibraryDatabase)

	public:

		enum class ArtistIDField : uint8_t
		{
			AlbumArtistID,
			ArtistID
		};

		LibraryDatabase(const QString& db_name, DbId db_id, LibraryId library_id);
		virtual ~LibraryDatabase();

		void change_artistid_field(ArtistIDField field);
		void clear();
		bool apply_fixes() override;

		LibraryId library_id() const;

		virtual bool store_metadata(const MetaDataList& v_md);
	};
}

#endif // LIBRARYDATABASE_H
