#ifndef PLAYLIST_TYPEDEFS_H
#define PLAYLIST_TYPEDEFS_H

#include <memory>
#include <QtGlobal>

class AbstractPlaylist;
/**
 * @brief PlaylistPtr Playlist Pointer (only used internally)
 * @ingroup Playlists
 */
typedef std::shared_ptr<AbstractPlaylist> PlaylistPtr;

/**
 * @brief PlaylistConstPtr read only Playlist Pointer
 * @ingroup Playlists
 */
typedef std::shared_ptr<const AbstractPlaylist> PlaylistConstPtr;


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

