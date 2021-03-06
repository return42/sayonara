/* CoverLookupAlternative.h */

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

#ifndef COVERLOOKUPALTERNATIVE_H
#define COVERLOOKUPALTERNATIVE_H

#include "AbstractCoverLookup.h"
#include "Utils/Pimpl.h"

namespace Cover
{
	class Location;

	/**
	 * @brief The CoverLookupAlternative class
	 * @ingroup Covers
	 */
	class AlternativeLookup :
			public LookupBase
	{
		Q_OBJECT
		PIMPL(AlternativeLookup)

	private:
		void go(const Location& cl);

	public:

		AlternativeLookup(QObject* parent, int n_covers);
		~AlternativeLookup();

		void start();
		void start(const QString& cover_fetcher_identifier);

		void start_text_search(const QString& search_term);
		void start_text_search(const QString& search_term, const QString& cover_fetcher_identifier);

		void stop() override;

		Cover::Location cover_location() const;
		void set_cover_location(const Cover::Location& location);


	private slots:
		void cover_found(const QString& cover_path);
		void finished(bool);
	};
}

#endif // COVERLOOKUPALTERNATIVE_H
