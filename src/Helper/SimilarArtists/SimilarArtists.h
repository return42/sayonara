#ifndef SIMILAR_ARTISTS_H
#define SIMILAR_ARTISTS_H

#include "Helper/singleton.h"
#include <QStringList>
#include <QMap>

class ArtistList;
class SimilarArtists
{
    SINGLETON(SimilarArtists)

public:
    QMap<QString, double> get_similar_artists(const QString& artist) const;
    QStringList get_similar_artist_names(const QString& artist) const;
};

#endif
