/* MyColumnHeader.h */

/* Copyright (C) 2013  Lucio Carreras
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
 * MyColumnHeader.h
 *
 *  Created on: 19.12.2012
 *      Author: lugmair
 */

#ifndef MYCOLUMNHEADER_H_
#define MYCOLUMNHEADER_H_

#include <QString>

#include "Helper/globals.h"
#include "Components/Library/Sorting.h"

enum ColHeaderSize {
	ColHeaderSizeAbs=0,
	ColHeaderSizeRel
};

class ColumnHeader {

	private:
		QString 		_title;

		bool 			_switchable;
		bool 			_abs_size;

		int 			_preferred_size_abs;
		double			_preferred_size_rel;

		SortOrder		_sort_asc;
		SortOrder		_sort_desc;

	public:
		ColumnHeader(QString title, bool switchable, SortOrder sort_asc, SortOrder sort_desc, int preferred_size_abs);
		ColumnHeader(QString title, bool switchable, SortOrder sort_asc, SortOrder sort_desc, double preferred_size_rel, int min_size);

		void set_preferred_size_abs(int preferred_size);

		void set_preferred_size_rel(double preferred_size);
		QString getTitle() const;
		bool getSwitchable() const;
		ColHeaderSize getSizeType() const;
		int get_preferred_size_abs() const;
		double get_preferred_size_rel() const;
		SortOrder get_asc_sortorder() const;
		SortOrder get_desc_sortorder() const;

};



#endif /* MYCOLUMNHEADER_H_ */
