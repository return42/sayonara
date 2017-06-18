/* LibraryInfo.h */

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

#ifndef LIBRARYINFO_H
#define LIBRARYINFO_H

#include "Helper/Pimpl.h"
#include <QtGlobal>

class QString;
class LibraryInfo
{
	PIMPL(LibraryInfo)

public:
	LibraryInfo();
	LibraryInfo(const QString& name, const QString& path, int id);
	LibraryInfo(const LibraryInfo& other);
	~LibraryInfo();

	LibraryInfo& operator =(const LibraryInfo& other);


	QString name() const;
	QString path() const;
	QString symlink_path() const;
	qint8 id() const;
	bool valid() const;

	static LibraryInfo fromString(const QString& str);
	QString toString() const;

	bool operator==(const LibraryInfo& other) const;
};

#endif // LIBRARYINFO_H
