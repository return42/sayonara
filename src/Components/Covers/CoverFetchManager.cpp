/* CoverFetchManager.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "CoverFetchManager.h"
#include "CoverFetcherInterface.h"
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
	QList<CoverFetcherInterface*> cover_fetcher;
	StandardCoverFetcher* std_cover_fetcher = nullptr;

	Private()
	{
		std_cover_fetcher = new StandardCoverFetcher();
		cover_fetcher << std_cover_fetcher;
	}

};

static QStringList sort_urls(QList<CoverFetchTupel>& lst)
{
	QStringList ret;
	/*std::sort(lst.begin(), lst.end(), [](const CoverFetchTupel& t1, const CoverFetchTupel& t2) {
		uint rating1 = (uint) (t1.rating);
		uint rating2 = (uint) (t2.rating);

		return (rating1 > rating2);
	});*/

	for(const CoverFetchTupel& t : lst){
		ret << t.url;
	}

	return ret;
}


CoverFetchManager::CoverFetchManager()
{
	_m = Pimpl::make<Private>();

	//_m->cover_fetcher << new GoogleCoverFetcher();
	//_m->cover_fetcher << new DiscogsCoverFetcher();
	_m->cover_fetcher << new LFMCoverFetcher();
	_m->cover_fetcher << _m->std_cover_fetcher;
}

CoverFetchManager::~CoverFetchManager() {}

void CoverFetchManager::register_cover_fetcher(CoverFetcherInterface *t)
{
	_m->cover_fetcher << t;
}

QStringList CoverFetchManager::get_artist_addresses(const QString& artist) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetcherInterface* acf : _m->cover_fetcher){
		if(acf->is_artist_supported()){
			urls << CoverFetchTupel(acf->get_artist_address(artist), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetchManager::get_album_addresses(const QString& artist, const QString& album) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetcherInterface* acf : _m->cover_fetcher){
		if(acf->is_album_supported()){
			urls << CoverFetchTupel(acf->get_album_address(artist, album), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

QStringList CoverFetchManager::get_search_addresses(const QString& str) const
{
	QList<CoverFetchTupel> urls;

	for(const CoverFetcherInterface* acf : _m->cover_fetcher){
		if(acf->is_search_supported()){
			urls << CoverFetchTupel(acf->get_search_address(str), acf->get_estimated_size());
		}
	}

	return sort_urls(urls);
}

CoverFetcherInterface* CoverFetchManager::get_coverfetcher(const QString& url) const
{
	for(CoverFetcherInterface* acf : _m->cover_fetcher)
	{
		QString keyword = acf->get_keyword();
		if(!keyword.isEmpty()){
			if(url.contains(keyword, Qt::CaseInsensitive)){
				return acf;
			}
		}
	}

	return _m->std_cover_fetcher;
}

