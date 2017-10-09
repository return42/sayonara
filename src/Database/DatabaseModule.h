/* DatabaseModule.h */

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

#ifndef DATABASEMODULE_H
#define DATABASEMODULE_H

#include <QSqlDatabase>
#include "Utils/Pimpl.h"

class SayonaraQuery;
class DatabaseModule
{
	PIMPL(DatabaseModule)

public:
	DatabaseModule(const QSqlDatabase& db, uint8_t db_id);
	virtual ~DatabaseModule();

	uint8_t module_db_id() const;
	QSqlDatabase module_db() const;
};

#endif // DATABASEMODULE_H
