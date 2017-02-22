#ifndef COVERFETCHMANAGER_H
#define COVERFETCHMANAGER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"

class CoverFetcherInterface;
class QStringList;
class QString;
class CoverFetchManager
{
	SINGLETON(CoverFetchManager)
	PIMPL(CoverFetchManager)

public:
	void register_cover_fetcher(CoverFetcherInterface* t);

	QStringList get_artist_addresses(const QString& artist) const;
	QStringList get_album_addresses(const QString& artist, const QString& album) const;
	QStringList get_search_addresses(const QString& str) const;

	CoverFetcherInterface* get_coverfetcher(const QString& url) const;
};


#endif // COVERFETCHMANAGER_H
