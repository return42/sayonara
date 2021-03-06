/* PlaylistFwd.h */

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

#ifndef PLAYLIST_TYPEDEFS_H
#define PLAYLIST_TYPEDEFS_H

#include <memory>
#include <QtGlobal>


namespace Playlist
{
	class Base;
}

/**
 * @brief PlaylistPtr Playlist Pointer (only used internally)
 * @ingroup Playlists
 * @ingroup PlaylistHelper
 */
typedef std::shared_ptr<Playlist::Base> PlaylistPtr;

/**
 * @brief PlaylistConstPtr read only Playlist Pointer
 * @ingroup Playlists
 * @ingroup PlaylistHelper
 */
typedef std::shared_ptr<const Playlist::Base> PlaylistConstPtr;



/**
 * @ingroup PlaylistHelper
 */
namespace Playlist
{
	class Mode;
	enum class Type : quint8
	{
		Std=0,
		Stream
	};
}

#endif

