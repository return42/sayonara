#include "Sorting.h"

#include <QStringList>

Library::Sortings::Sortings()
{
    so_artists = Library::SortOrder::ArtistNameAsc;
    so_albums = Library::SortOrder::AlbumNameAsc;
    so_tracks = Library::SortOrder::TrackAlbumAsc;
}

Library::Sortings::Sortings(const Sortings& so)
{
    so_albums = so.so_albums;
    so_artists = so.so_artists;
    so_tracks = so.so_tracks;
}

Library::Sortings::~Sortings(){}

bool Library::Sortings::operator==(Library::Sortings so)
{
    return  (so.so_albums == so_albums) &&
	    (so.so_artists == so_artists) &&
	    (so.so_tracks == so_tracks);
}


QString Library::Sortings::toString() const
{
    return QString::number((int) so_albums) + "," +
	    QString::number((int) so_artists) + "," +
	    QString::number((int) so_tracks);
}


Library::Sortings Library::Sortings::fromString(const QString& str)
{
    Library::Sortings so;
    QStringList lst = str.split(",");
	    so.so_albums = (Library::SortOrder) lst[0].toInt();
	    so.so_artists = (Library::SortOrder) lst[1].toInt();
	    so.so_tracks = (Library::SortOrder) lst[2].toInt();
    return so;
}
