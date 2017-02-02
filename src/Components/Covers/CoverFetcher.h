#ifndef COVERFETCHER_H
#define COVERFETCHER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"

class CoverFetchThread;
class QStringList;
class QString;
class CoverFetcher
{
    SINGLETON(CoverFetcher)
    PIMPL(CoverFetcher)

    public:
	void register_cover_fetcher(CoverFetchThread* t);

	QStringList get_artist_addresses(const QString& artist) const;
	QStringList get_album_addresses(const QString& artist, const QString& album) const;
	QStringList get_search_addresses(const QString& str) const;
};


#endif // COVERFETCHER_H
