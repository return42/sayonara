/* PlaylistLoader.h */

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

#ifndef PLAYLISTLOADER_H
#define PLAYLISTLOADER_H

#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Playlist/CustomPlaylistFwd.h"
#include "Utils/Pimpl.h"

#include <QObject>

namespace Playlist
{
	class DBWrapper;

	/**
	 * @brief The PlaylistLoader class
	 * @ingroup Playlists
	 */
	class Loader :
			public QObject,
			public SayonaraClass
	{
		Q_OBJECT
		PIMPL(Loader)

		public:
			explicit Loader(QObject* parent=nullptr);
			virtual ~Loader();


			CustomPlaylists			get_playlists() const;
			int						get_last_playlist_idx() const;
			int						get_last_track_idx() const;
			int						create_playlists();
	};
}

#endif // PLAYLISTLOADER_H
