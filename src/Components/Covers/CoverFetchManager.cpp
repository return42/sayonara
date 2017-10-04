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
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QStringList>
#include <QList>
#include <QMap>
#include <algorithm>

struct CoverFetchManager::Private
{
	QMap<QString, int> cf_order;
	QList<CoverFetcherInterface*> coverfetchers;
	QList<CoverFetcherInterface*> active_coverfetchers;
	StandardCoverFetcher* std_cover_fetcher = nullptr;
};


static void sort_coverfetchers(QList<CoverFetcherInterface*>& lst, const QMap<QString, int>& cf_order)
{
	std::sort(lst.begin(), lst.end(), [&cf_order](CoverFetcherInterface* t1, CoverFetcherInterface* t2) {

		int order1 = cf_order[t1->get_keyword()];
		int order2 = cf_order[t2->get_keyword()];
		if(order1 != order2) {
			if(order1 == -1){
				return false; // order1 is worse
			}

			if(order2 == -1){
				return true; // order1 is better
			}

			return (order1 < order2);
		}

		int rating1 = t1->get_estimated_size();
		int rating2 = t2->get_estimated_size();

		return (rating1 > rating2);
	});
}


static CoverFetcherInterface* get_coverfetcher_by_keyword(const QString& keyword, const QList<CoverFetcherInterface*>& container)
{
	if(keyword.isEmpty()){
		return nullptr;
	}

	for(CoverFetcherInterface* cfi : container)
	{
		QString cfi_keyword = cfi->get_keyword();
		if(!cfi_keyword.isEmpty()){
			if(cfi_keyword.compare(keyword, Qt::CaseInsensitive) == 0){
				return cfi;
			}
		}
	}

	return nullptr;
}

static CoverFetcherInterface* get_coverfetcher_by_url(const QString& url, const QList<CoverFetcherInterface*>& container)
{
	if(url.isEmpty()){
		return nullptr;
	}

	for(CoverFetcherInterface* cfi : container)
	{
		QString keyword = cfi->get_keyword();
		if(!keyword.isEmpty()){
			if(url.contains(keyword, Qt::CaseInsensitive)){
				return cfi;
			}
		}
	}

	return nullptr;
}


CoverFetchManager::CoverFetchManager() :
	QObject(),
	SayonaraClass()
{
	m = Pimpl::make<Private>();
	m->std_cover_fetcher = new StandardCoverFetcher();

	register_coverfetcher(new GoogleCoverFetcher());
	register_coverfetcher(new DiscogsCoverFetcher());
	register_coverfetcher(new LFMCoverFetcher());
	register_coverfetcher(m->std_cover_fetcher);

    Set::listen(Set::Cover_Server, this, &CoverFetchManager::active_changed);
}

CoverFetchManager::~CoverFetchManager() {}

void CoverFetchManager::register_coverfetcher(CoverFetcherInterface *t)
{
	CoverFetcherInterface* cfi = get_coverfetcher_by_keyword(t->get_keyword(), m->coverfetchers);
	if(cfi){
		return;
	}

	m->coverfetchers << t;
}


void CoverFetchManager::activate_coverfetchers(const QStringList& coverfetchers)
{
	m->active_coverfetchers.clear();
	m->cf_order.clear();
	m->cf_order[""] = 100;

	int idx = 0;
	for(const QString& coverfetcher : coverfetchers){
		CoverFetcherInterface* cfi = get_coverfetcher_by_keyword(coverfetcher, m->coverfetchers);
		if(cfi){
			m->active_coverfetchers << cfi;
			m->cf_order[cfi->get_keyword()] = idx;
			idx++;
		}
	}

	if(m->active_coverfetchers.isEmpty()){
		m->active_coverfetchers << get_coverfetcher_by_keyword("google", m->coverfetchers);
	}

	m->active_coverfetchers << m->std_cover_fetcher;
	sort_coverfetchers(m->active_coverfetchers, m->cf_order);

	for(const CoverFetcherInterface* cfi : m->active_coverfetchers){
		sp_log(Log::Debug, this) << "Active Coverfetcher: " << cfi->get_keyword();
	}
}

CoverFetcherInterface* CoverFetchManager::get_available_coverfetcher(const QString& url) const
{
	CoverFetcherInterface* cfi = get_coverfetcher_by_url(url, m->coverfetchers);
	if(!cfi){
		return m->std_cover_fetcher;
	}

	return cfi;
}

CoverFetcherInterface* CoverFetchManager::get_active_coverfetcher(const QString& url) const
{
	CoverFetcherInterface* cfi = get_coverfetcher_by_url(url, m->active_coverfetchers);
	if(!cfi){
		return m->std_cover_fetcher;
	}

	return cfi;
}


QList<CoverFetcherInterface*> CoverFetchManager::get_available_coverfetchers() const
{
	return m->coverfetchers;
}

QList<CoverFetcherInterface*> CoverFetchManager::get_active_coverfetchers() const
{
	return m->active_coverfetchers;
}

void CoverFetchManager::active_changed()
{
	QStringList active = _settings->get(Set::Cover_Server);
	activate_coverfetchers(active);
}



QStringList CoverFetchManager::get_artist_addresses(const QString& artist) const
{
	QStringList urls;

	for(const CoverFetcherInterface* cfi : m->active_coverfetchers){
		if(cfi->is_artist_supported()){
			urls << cfi->get_artist_address(artist);
		}
	}

	return urls;
}

QStringList CoverFetchManager::get_album_addresses(const QString& artist, const QString& album) const
{
	QStringList urls;

	for(const CoverFetcherInterface* cfi : m->active_coverfetchers){
		if(cfi->is_album_supported()){
			urls << cfi->get_album_address(artist, album);
		}
	}

	return urls;
}

QStringList CoverFetchManager::get_search_addresses(const QString& str) const
{
	QStringList urls;

	for(const CoverFetcherInterface* cfi : m->active_coverfetchers){
		if(cfi->is_search_supported()){
			urls << cfi->get_search_address(str);
		}
	}

	return urls;
}
