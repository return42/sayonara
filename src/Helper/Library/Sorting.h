/* Sorting.h */

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

#ifndef LIBRARY_SORTING_H
#define LIBRARY_SORTING_H

#include <QtGlobal>
#include "Sortorder.h"

class QString;
namespace Library
{

    class Sortings
    {

    public:
	Library::SortOrder so_albums;
	Library::SortOrder so_artists;
	Library::SortOrder so_tracks;

	Sortings();
	Sortings(const Sortings& so);
	~Sortings();

	bool operator==(Library::Sortings so);

	QString toString() const;

	static Library::Sortings fromString(const QString& str);
    };
}

#endif // SORTING_H
