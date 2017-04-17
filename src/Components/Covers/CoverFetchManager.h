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

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"

class CoverFetcherInterface;
class QStringList;
class QString;

/**
 * @brief Retrieve Download Urls for Cover Searcher.
 * CoverFetcherInterface can be registered, so for
 * example a last.fm cover fetcher via the register_cover_fetcher
 * method. A specific CoverFetcherInterface may be retrieved by using
 * the get_coverfetcher method.
 * @ingroup Covers
 */
class CoverFetchManager
{
	SINGLETON(CoverFetchManager)
	PIMPL(CoverFetchManager)

public:
	/**
	 * @brief Register a cover fetcher. Per default
	 * there is one for Discogs, last.fm and Google
	 * @param t an instance of a CoverFetcherInterface
	 */
	void register_cover_fetcher(CoverFetcherInterface* t);

	/**
	 * @brief get urls for a artist search query
	 * @param artist name
	 * @return list of urls
	 */
	QStringList get_artist_addresses(const QString& artist) const;

	/**
	 * @brief get urls for a album search query
	 * @param artist artist name
	 * @param album album name
	 * @return list of urls
	 */
	QStringList get_album_addresses(const QString& artist, const QString& album) const;

	/**
	 * @brief get urls for a fuzzy query
	 * @param str query string
	 * @return list of urls
	 */
	QStringList get_search_addresses(const QString& str) const;

	/**
	 * @brief get a CoverFetcherInterface by a specific url
	 * @param url cover search url
	 * @return null, if there's no suitable CoverFetcherInterface registered
	 */
	CoverFetcherInterface* get_coverfetcher(const QString& url) const;
};


#endif // COVERFETCHMANAGER_H
