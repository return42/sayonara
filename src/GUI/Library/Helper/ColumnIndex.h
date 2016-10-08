#ifndef LIBRARYVIEWCOLUMNS_H
#define LIBRARYVIEWCOLUMNS_H

#define COL_ALBUM_MACROS
#define COL_ARTIST_MACROS

namespace ColumnIndex
{
enum class Album : quint8
{
    MultiDisc=0,
    Name=1,
    Duration=2,
    NumSongs=3,
    Year=4,
    Rating=5
};

enum class Artist : quint8
{
    NumAlbums=0,
    Name=1,
    Tracks=2
};

enum class Track : quint8
{
    TrackNumber=0,
    Title=1,
    Artist=2,
    Album=3,
    Year=4,
    Length=5,
    Bitrate=6,
    Filesize=7,
    Rating=8
};
}




#endif // LIBRARYVIEWCOLUMNS_H
