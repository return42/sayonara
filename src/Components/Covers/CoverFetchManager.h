/* CoverFetchManager.h */

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



#ifndef COVERFETCHMANAGER_H
#define COVERFETCHMANAGER_H

#include "Utils/Singleton.h"
#include "Utils/Pimpl.h"
#include "Utils/Settings/SayonaraClass.h"
#include <QList>
#include <QObject>

class QStringList;
class QString;

namespace Cover
{
namespace Fetcher
{

	class Base;
	/**
	 * @brief Retrieve Download Urls for Cover Searcher.
	 * CoverFetcherInterface can be registered, so for
	 * example a last.fm cover fetcher via the register_cover_fetcher
	 * method. A specific CoverFetcherInterface may be retrieved by using
	 * the get_coverfetcher method.
	 * @ingroup Covers
	 */
	class Manager :
			public QObject,
			public SayonaraClass
	{
		Q_OBJECT

		SINGLETON(Manager)
		PIMPL(Manager)

	public:
		/**
		 * @brief Register a cover fetcher. Per default
		 * there is one for Discogs, last.fm and Google
		 * @param t an instance of a CoverFetcherInterface
		 */
		void register_coverfetcher(Cover::Fetcher::Base* t);

		/**
		 * @brief activate coverfetchers by their keywords
		 * @param coverfetchers list of cover fetcher keywords.
		 */
		void activate_coverfetchers(const QStringList& coverfetchers);

		/**
		 * @brief get urls for a artist search query
		 * @param artist name
		 * @return list of urls
		 */
		QStringList artist_addresses(const QString& artist) const;

		/**
		 * @brief get urls for a album search query
		 * @param artist artist name
		 * @param album album name
		 * @return list of urls
		 */
		QStringList album_addresses(const QString& artist, const QString& album) const;

		/**
		 * @brief get urls for a fuzzy query
		 * @param str query string
		 * @return list of urls
		 */
		QStringList search_addresses(const QString& str) const;

		/**
		 * @brief get a CoverFetcherInterface by a specific url
		 * @param url cover search url
		 * @return null, if there's no suitable CoverFetcherInterface registered
		 */
		Cover::Fetcher::Base* available_coverfetcher(const QString& url) const;

		/**
		 * @brief get an activated coverfetcher
		 * @param url cover search url
		 * @return null, if there's no suitable CoverFetcherInterface activated
		 */
		Cover::Fetcher::Base* active_coverfetcher(const QString& url) const;


		/**
		 * @brief fetches all available cover fetcher
		 * @return
		 */
		QList<Cover::Fetcher::Base*> available_coverfetchers() const;

		/**
		 * @brief fetches all active cover fetchers
		 * @return
		 */
		QList<Cover::Fetcher::Base*> active_coverfetchers() const;


	private slots:
		void servers_changed();
	};

}
}
#endif // COVERFETCHMANAGER_H
