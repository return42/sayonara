#include "CoverFetcher.h"
#include "CoverFetchThread.h"
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


struct CoverFetcher::Private
{
	QList<CoverFetchThread*> cover_fetcher;
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


CoverFetcher::CoverFetcher()
{
	_m = Pimpl::make<Private>();
	_m->cover_fetcher << new DiscogsCoverFetcher();
	_m->cover_fetcher << new LFMCoverFetcher();
	_m->cover_fetcher << new GoogleCoverFetcher();
	_m->cover_fetcher << new StandardCoverFetcher();
}

CoverFetcher::~CoverFetcher() {}

void CoverFetcher::register_cover_fetcher(CoverFetchThread *t)
{
	_m->cover_fetcher << t;
}

QStringList CoverFetcher::get_artist_addresses(const QString& artist) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetchThread* cft : _m->cover_fetcher){
		if(cft->is_artist_supported()){
			urls << CoverFetchTupel(cft->get_artist_address(artist), cft->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetcher::get_album_addresses(const QString& artist, const QString& album) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetchThread* cft : _m->cover_fetcher){
		if(cft->is_album_supported()){
			urls << CoverFetchTupel(cft->get_album_address(artist, album), cft->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetcher::get_search_addresses(const QString& str) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetchThread* cft : _m->cover_fetcher){
		if(cft->is_search_supported()){
			urls << CoverFetchTupel(cft->get_search_address(str), cft->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

