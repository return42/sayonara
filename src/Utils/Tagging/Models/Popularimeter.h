/* Popularimeter.h */

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



#ifndef POPULARIMETER_MODELS_H_
#define POPULARIMETER_MODELS_H_

#include <QString>

namespace Models
{
    /**
     * @brief The Popularimeter class
     * @ingroup Tagging
     */
    class Popularimeter
    {
	public:

	    QString		email;
		int			rating;
		int			playcount;

	    Popularimeter();
		Popularimeter(const QString& email_, uint8_t rating_byte_, int playcount);
		void set_rating(uint8_t max_5);
		void set_rating_byte(uint8_t byte);
		uint8_t get_rating() const;
		uint8_t get_rating_byte() const;
	    QString to_string();
    };
}

#endif // POPULARIMETER_H
