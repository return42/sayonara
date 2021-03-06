/* MyColumnHeader.h */

/* Copyright 2011-2017  Lucio Carreras
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
 *
 *  Created on: 19.12.2012
 *      Author: luke
 */

#ifndef MYCOLUMNHEADER_H_
#define MYCOLUMNHEADER_H_

#include <QObject>

#include "Utils/Library/Sortorder.h"
#include "Utils/Pimpl.h"

class QAction;

class ColumnHeader : public QObject
{
	Q_OBJECT
	PIMPL(ColumnHeader)

	public:
		enum class SizeType : uint8_t
		{
			Abs=0,
			Rel,
			Undefined
		};

		enum HeaderType
		{
			Sharp,
			Artist,
			Album,
			Title,
			NumTracks,
			Duration,
			DurationShort,
			Year,
			Rating,
			Bitrate,
			Filesize
		};

	private:
		ColumnHeader(HeaderType type, bool switchable, Library::SortOrder sort_asc, Library::SortOrder sort_desc);
		virtual ~ColumnHeader();

	public:
		ColumnHeader(HeaderType type, bool switchable, Library::SortOrder sort_asc, Library::SortOrder sort_desc, int preferred_size_abs);
		ColumnHeader(HeaderType type, bool switchable, Library::SortOrder sort_asc, Library::SortOrder sort_desc, double preferred_size_rel, int min_size);

		int preferred_size_abs() const;
		double preferred_size_rel() const;

		Library::SortOrder sortorder_asc() const;
		Library::SortOrder sortorder_desc() const;

		ColumnHeader::SizeType size_type() const;

		bool is_visible() const;
		bool is_hidden() const;

		void retranslate();

		QAction* action();
		QString title() const;
};

class ColumnHeaderList :
	public QList<ColumnHeader*>
{
	public:
	   int visible_columns() const;
	   int visible_column(int n) const;
};

#endif /* MYCOLUMNHEADER_H_ */
