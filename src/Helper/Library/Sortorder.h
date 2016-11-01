#ifndef SORTORDER_H_
#define SORTORDER_H_

#include <QtGlobal>

namespace Library
{
    enum class SortOrder : quint8
    {
	    NoSorting=0,
	    ArtistNameAsc,
	    ArtistNameDesc,
	    ArtistTrackcountAsc,
	    ArtistTrackcountDesc,
	    AlbumNameAsc,
	    AlbumNameDesc,
	    AlbumYearAsc,
	    AlbumYearDesc,
	    AlbumTracksAsc,
	    AlbumTracksDesc,
	    AlbumDurationAsc,
	    AlbumDurationDesc,
	    AlbumRatingAsc,
	    AlbumRatingDesc,
	    TrackNumAsc,
	    TrackNumDesc,
	    TrackTitleAsc,
	    TrackTitleDesc,
	    TrackAlbumAsc,
	    TrackAlbumDesc,
	    TrackArtistAsc,
	    TrackArtistDesc,
	    TrackYearAsc,
	    TrackYearDesc,
	    TrackLenghtAsc,
	    TrackLengthDesc,
	    TrackBitrateAsc,
	    TrackBitrateDesc,
	    TrackSizeAsc,
	    TrackSizeDesc,
	    TrackDiscnumberAsc,
	    TrackDiscnumberDesc,
	    TrackRatingAsc,
	    TrackRatingDesc
    };
}

#endif
