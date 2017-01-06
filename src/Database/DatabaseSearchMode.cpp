/* DatabaseSearchMode.cpp */

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

#include "Database/DatabaseSearchMode.h"
#include "Database/SayonaraQuery.h"


struct DatabaseSearchMode::Private
{
	QSqlDatabase db;
	bool initialized;
	Library::SearchModeMask search_mode;


	Private()
	{
		initialized = false;
		search_mode = Library::CaseInsensitve;
	}
};

DatabaseSearchMode::DatabaseSearchMode()
{
	_m = Pimpl::make<DatabaseSearchMode::Private>();
}

DatabaseSearchMode::DatabaseSearchMode(const QSqlDatabase& db) :
	DatabaseSearchMode()
{
	_m->db = db;
}

DatabaseSearchMode::~DatabaseSearchMode(){}


void DatabaseSearchMode::init()
{
	if(_m->initialized){
		return;
	}

	SayonaraQuery q(_m->db);
	q.prepare("SELECT value FROM settings WHERE key = 'lib_search_mode';");

	if(q.exec()) {
		if (q.next()) {
			_m->initialized = true;
			_m->search_mode = q.value(0).toInt();
		}
	}
}

Library::SearchModeMask DatabaseSearchMode::search_mode()
{
	init();

	return _m->search_mode;
}

void DatabaseSearchMode::update_search_mode()
{
	_m->initialized = false;
	init();
}
