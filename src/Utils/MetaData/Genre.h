/* Genre.h */

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

#ifndef GENRE_H
#define GENRE_H

#include <QString>
#include "Utils/Pimpl.h"

class Genre
{
private:
	PIMPL(Genre)

public:
	Genre();
	explicit Genre(const QString& name);
	Genre(const Genre& other);

	~Genre();

	static uint32_t calc_id(const QString& name);
	uint32_t id() const;

	QString name() const;
	void set_name(const QString& name);

	bool is_equal(const Genre& other) const;
	bool operator ==(const Genre& other) const;
	bool operator <(const Genre& other) const;
	bool operator >(const Genre& other) const;
	Genre& operator =(const Genre& other);
};


#endif // GENRE_H
