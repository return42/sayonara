#include "CoverFetcher.h"
#include "CoverFetchThread.h"
#include "AbstractCoverFetcher.h"
#include "GoogleCoverFetcher.h"
#include "LFMCoverFetcher.h"
#include "StandardCoverFetcher.h"
#include "DiscogsCoverFetcher.h"

#include <QStringList>

struct CoverFetchTupel
{
	QString url;
	int rating;

	CoverFetchTupel(){
		rating = 0;
	}

	CoverFetchTupel(const QString& url_, int rating_)
	{
		url = url_;
		rating = rating_;
	}
};


struct CoverFetchManager::Private
{
	QList<AbstractCoverFetcher*> cover_fetcher;
};

static QStringList sort_urls(QList<CoverFetchTupel>& lst)
{
	QStringList ret;
	std::sort(lst.begin(), lst.end(), [](const CoverFetchTupel& t1, const CoverFetchTupel& t2){
		uint rating1 = (uint) (t1.rating);
		uint rating2 = (uint) (t2.rating);

		return (rating1 > rating2);
	});

	for(const CoverFetchTupel& t : lst){
		ret << t.url;
	}

	return ret;
}


CoverFetchManager::CoverFetchManager()
{
	_m = Pimpl::make<Private>();
	_m->cover_fetcher << new DiscogsCoverFetcher();
	_m->cover_fetcher << new LFMCoverFetcher();
	_m->cover_fetcher << new GoogleCoverFetcher();
	_m->cover_fetcher << new StandardCoverFetcher();
}

CoverFetchManager::~CoverFetchManager() {}

void CoverFetchManager::register_cover_fetcher(AbstractCoverFetcher *t)
{
	_m->cover_fetcher << t;
}

QStringList CoverFetchManager::get_artist_addresses(const QString& artist) const
{
	QList<CoverFetchTupel> urls;

	for(const AbstractCoverFetcher* acf : _m->cover_fetcher){
		if(acf->is_artist_supported()){
			urls << CoverFetchTupel(acf->get_artist_address(artist), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetchManager::get_album_addresses(const QString& artist, const QString& album) const
{
	QList<CoverFetchTupel> urls;

	for(const AbstractCoverFetcher* acf : _m->cover_fetcher){
		if(acf->is_album_supported()){
			urls << CoverFetchTupel(acf->get_album_address(artist, album), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetchManager::get_search_addresses(const QString& str) const
{
	QList<CoverFetchTupel> urls;

	for(const AbstractCoverFetcher* acf : _m->cover_fetcher){
		if(acf->is_search_supported()){
			urls << CoverFetchTupel(acf->get_search_address(str), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

AbstractCoverFetcher* CoverFetchManager::get_coverfetcher(const QString& url) const
{
	for(AbstractCoverFetcher* acf : _m->cover_fetcher)
	{
		QString keyword = acf->get_keyword();
		if(!keyword.isEmpty()){
			if(url.contains(keyword, Qt::CaseInsensitive)){
				return acf;
			}
		}

		else {
			if(url.isEmpty()){
				return acf;
			}
		}
	}

	return nullptr;
}

