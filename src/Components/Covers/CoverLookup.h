/* CoverLookup.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * CoverLookup.h
 *
 *  Created on: Apr 4, 2011
 *      Author: Lucio Carreras
 */

#ifndef COVERLOOKUP_H_
#define COVERLOOKUP_H_

#include "AbstractCoverLookup.h"
#include "Helper/Pimpl.h"

class Album;
class CoverLocation;

/**
 * @brief The CoverLookup class
 * @ingroup Covers
 */
class CoverLookup :
		public AbstractCoverLookup
{
	Q_OBJECT
	PIMPL(CoverLookup)


public:

	CoverLookup(QObject* parent=nullptr, int n_covers=1);
	~CoverLookup();


	/**
	 * @brief fetches cover for a CoverLocation.
	 *   1. Looks up CoverLocation::cover_path
	 *   2. Looks up CoverLocation::local_paths
	 *   3. Starts a CoverFetchThread
	 * @param cl CoverLocation of interest
	 * @return always true
	 */
	bool fetch_cover(const CoverLocation& cl, bool also_www=true);

	/**
	 * @brief uses CoverLocation(const Album& album)
	 * @param album Album object
	 * @return true
	 */
	bool fetch_album_cover(const Album& album, bool also_www=true);


	/**
	 * @brief not implemented
	 */
	void stop() override;


private:
	/**
	 * @brief Starts a new CoverFetchThread
	 * @param cl CoverLocation object
	 */
	bool start_new_thread(const CoverLocation& cl);


private slots:
	/**
	 * @brief called when CoverFetchThread has found cover
	 * @param cl
	 */
	void cover_found(const QString& cover_path);

	/**
	 * @brief called when CoverFetchThread has finished
	 */
	void finished(bool);
};

/**
 * @brief CoverLookupPtr
 * @ingroup Covers
 */
using CoverLookupPtr=std::shared_ptr<CoverLookup>;

#endif /* COVERLOOKUP_H_ */
