#ifndef COVERFETCHER_H
#define COVERFETCHER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"
#include <memory>

class CoverFetchThread;
class QStringList;
class QString;
class CoverFetcher
{
    PIMPL(CoverFetcher)


public:
	CoverFetcher();
	~CoverFetcher();

	void register_cover_fetcher(CoverFetchThread* t);

	QStringList get_artist_addresses(const QString& artist) const;
	QStringList get_album_addresses(const QString& artist, const QString& album) const;
	QStringList get_search_addresses(const QString& str) const;

	CoverFetchThread* get_by_url(const QString& url) const;
};

typedef std::shared_ptr<CoverFetcher> CoverFetcherPtr;


#endif // COVERFETCHER_H
