#ifndef METADATA_FWD_H
#define METADATA_FWD_H

class MetaData;
class MetaDataList;
class Artist;
class Album;
class ArtistList;
class AlbumList;

namespace MD 
{
	enum class Interpretation : char
	{
		None=0,
		Artists,
		Albums,
		Tracks
	};
}
#endif
