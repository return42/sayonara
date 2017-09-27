/* DatabaseModule.cpp */

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

#include "Database/DatabaseModule.h"

struct DatabaseModule::Private
{
	QSqlDatabase db;
	uint8_t db_id;

	Private(const QSqlDatabase& db, uint8_t db_id) :
		db(db),
		db_id(db_id)
	{}
};

DatabaseModule::DatabaseModule(const QSqlDatabase& db, uint8_t db_id)
{
	m = Pimpl::make<Private>(db, db_id);
}

DatabaseModule::~DatabaseModule() {}

uint8_t DatabaseModule::module_db_id() const
{
	return m->db_id;
}

QSqlDatabase DatabaseModule::module_db() const
{
	return m->db;
}


