#ifndef SORTING_H
#define SORTING_H

#include "Helper/Library/Sorting.h"

class ArtistList;
class AlbumList;
class MetaDataList;

namespace SC
{
	namespace Sorting
	{
		void sort_artists(ArtistList& artists, ::Library::SortOrder so);
		void sort_albums(AlbumList& albums, ::Library::SortOrder so);
		void sort_tracks(MetaDataList& v_md, ::Library::SortOrder so);
	};
}

#endif // SORTING_H
