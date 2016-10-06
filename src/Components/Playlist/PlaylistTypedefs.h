#ifndef PLAYLIST_TYPEDEFS_H
#define PLAYLIST_TYPEDEFS_H

#include <memory>
#include <QtGlobal>

class Playlist;
/**
 * @brief PlaylistPtr Playlist Pointer (only used internally)
 * @ingroup Playlists
 */
typedef std::shared_ptr<Playlist> PlaylistPtr;

/**
 * @brief PlaylistConstPtr read only Playlist Pointer
 * @ingroup Playlists
 */
typedef std::shared_ptr<const Playlist> PlaylistConstPtr;



enum class PlaylistType : quint8
{
    Std=0,
    Stream
};

#endif

