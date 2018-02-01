/* Manager.cpp */

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

#include "Utils/Utils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <QStringList>
#include <QList>
#include <QMap>
#include <algorithm>

using namespace Cover;
using Cover::Fetcher::Manager;

struct Manager::Private
{
	QMap<QString, int>		cf_order;
	QList<Fetcher::Base*>	coverfetchers;
	QList<Fetcher::Base*>	active_coverfetchers;
	Fetcher::Standard*		std_cover_fetcher = nullptr;
};

static void sort_coverfetchers(QList<Fetcher::Base*>& lst, const QMap<QString, int>& cf_order)
{
	std::sort(lst.begin(), lst.end(), [&cf_order](Fetcher::Base* t1, Fetcher::Base* t2)
	{
		int order1 = cf_order[t1->keyword()];
		int order2 = cf_order[t2->keyword()];
		if(order1 != order2) {
			if(order1 == -1){
				return false; // order1 is worse
			}

			if(order2 == -1){
				return true; // order1 is better
			}

			return (order1 < order2);
		}

		int rating1 = t1->estimated_size();
		int rating2 = t2->estimated_size();

		return (rating1 > rating2);
	});
}


static Fetcher::Base* coverfetcher_by_keyword(const QString& keyword, const QList<Fetcher::Base*>& container)
{
	if(keyword.isEmpty()){
		return nullptr;
	}

	for(Fetcher::Base* cfi : container)
	{
		QString cfi_keyword = cfi->keyword();
		if(!cfi_keyword.isEmpty()){
			if(cfi_keyword.compare(keyword, Qt::CaseInsensitive) == 0){
				return cfi;
			}
		}
	}

	return nullptr;
}

static Fetcher::Base* coverfetcher_by_url(const QString& url, const QList<Fetcher::Base*>& container)
{
	if(url.isEmpty()){
		return nullptr;
	}

	for(Fetcher::Base* cfi : container)
	{
		QString keyword = cfi->keyword();
		if(!keyword.isEmpty()){
			if(url.contains(keyword, Qt::CaseInsensitive)){
				return cfi;
			}
		}
	}

	return nullptr;
}


Manager::Manager() :
	QObject(),
	SayonaraClass()
{
	m = Pimpl::make<Private>();
	m->std_cover_fetcher = new Standard();

	register_coverfetcher(new Fetcher::Google());
	register_coverfetcher(new Fetcher::Discogs());
	register_coverfetcher(new Fetcher::LastFM());
	register_coverfetcher(m->std_cover_fetcher);

	Set::listen<Set::Cover_Server>(this, &Manager::servers_changed);
}

Manager::~Manager() {}

void Manager::register_coverfetcher(Base *t)
{
	Fetcher::Base* cfi = coverfetcher_by_keyword(t->keyword(), m->coverfetchers);
	if(cfi){
		return;
	}

	m->coverfetchers << t;
}


void Manager::activate_coverfetchers(const QStringList& coverfetchers)
{
	m->active_coverfetchers.clear();
	m->cf_order.clear();
	m->cf_order[""] = 100;

	int idx = 0;
	for(const QString& coverfetcher : coverfetchers)
	{
		Fetcher::Base* cfi = coverfetcher_by_keyword(coverfetcher, m->coverfetchers);
		if(cfi)
		{
			m->active_coverfetchers << cfi;
			m->cf_order[cfi->keyword()] = idx;
			idx++;
		}
	}

	if(m->active_coverfetchers.isEmpty()){
		m->active_coverfetchers << coverfetcher_by_keyword("google", m->coverfetchers);
	}

	m->active_coverfetchers << m->std_cover_fetcher;
	sort_coverfetchers(m->active_coverfetchers, m->cf_order);

	for(const Fetcher::Base* cfi : ::Util::AsConst(m->active_coverfetchers))
	{
		if(!cfi->keyword().isEmpty()) {
			sp_log(Log::Debug, this) << "Active Coverfetcher: " << cfi->keyword();
		}
	}
}

Fetcher::Base* Manager::available_coverfetcher(const QString& url) const
{
	Fetcher::Base* cfi = coverfetcher_by_url(url, m->coverfetchers);
	if(!cfi){
		return m->std_cover_fetcher;
	}

	return cfi;
}

Fetcher::Base* Manager::active_coverfetcher(const QString& url) const
{
	Fetcher::Base* cfi = coverfetcher_by_url(url, m->active_coverfetchers);
	if(!cfi){
		return m->std_cover_fetcher;
	}

	return cfi;
}


QList<Fetcher::Base*> Manager::available_coverfetchers() const
{
	return m->coverfetchers;
}

QList<Fetcher::Base*> Manager::active_coverfetchers() const
{
	return m->active_coverfetchers;
}

void Manager::servers_changed()
{
	QStringList servers = _settings->get<Set::Cover_Server>();
	activate_coverfetchers(servers);
}



QStringList Manager::artist_addresses(const QString& artist) const
{
	QStringList urls;

	for(const Fetcher::Base* cfi : ::Util::AsConst(m->active_coverfetchers))
	{
		if(cfi->is_artist_supported())
		{
			urls << cfi->artist_address(artist);
		}
	}

	return urls;
}

QMap<QString, QString> Manager::all_artist_addresses(const QString &artist) const
{
	QMap<QString, QString> url_map;
	for(const Fetcher::Base* cfi : ::Util::AsConst(m->coverfetchers))
	{
		if(cfi->is_artist_supported())
		{
			url_map[cfi->keyword()] = cfi->artist_address(artist);
		}
	}

	return url_map;
}

QStringList Manager::album_addresses(const QString& artist, const QString& album) const
{
	QStringList urls;

	for(const Fetcher::Base* cfi : ::Util::AsConst(m->active_coverfetchers))
	{
		if(cfi->is_album_supported()){
			urls << cfi->album_address(artist, album);
		}
	}

	return urls;
}

QMap<QString, QString> Manager::all_album_addresses(const QString &artist, const QString &album) const
{
	QMap<QString, QString> url_map;
	for(const Fetcher::Base* cfi : ::Util::AsConst(m->coverfetchers))
	{
		if(cfi->is_album_supported())
		{
			url_map[cfi->keyword()] = cfi->album_address(artist, album);
		}
	}

	return url_map;
}

QStringList Manager::search_addresses(const QString& str) const
{
	QStringList urls;

	for(const Fetcher::Base* cfi : ::Util::AsConst(m->active_coverfetchers))
	{
		if(cfi->is_search_supported())
		{
			urls << cfi->search_address(str);
		}
	}

	return urls;
}

QStringList Manager::search_addresses(const QString &str, const QString& cover_fetcher_identifier) const
{
	QStringList urls;

	for(const Fetcher::Base* cfi : ::Util::AsConst(m->coverfetchers))
	{
		if( (cfi->is_search_supported()) &&
			(cover_fetcher_identifier.compare(cfi->keyword()) == 0))
		{
			urls << cfi->search_address(str);
		}
	}

	if(urls.isEmpty()){
		return search_addresses(str);
	}

	return urls;
}
