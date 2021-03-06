/* Cover.h */

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



#ifndef COVER_MODELS_H_
#define COVER_MODELS_H_

#include <QString>

/**
 * @ingroup Tagging
 */
namespace Models
{
	/**
	 * @brief The Cover class
	 * @ingroup Tagging
	 */
    class Cover
    {
		private:
			QString description;

		public:
			enum class MimeType
			{
				JPEG,
				PNG,
				Unsupported
			};


			QString mime_type;
			QByteArray image_data;

			Cover();
			Cover(const QString& mime_type, const QByteArray& image_data);
			MimeType get_mime_type() const;
    };
}

#endif // COVER_H
