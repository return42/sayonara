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
#include "Utils/Settings/Settings.h"

using DB::SearchMode;

struct SearchMode::Private
{
	bool initialized;
	Library::SearchModeMask search_mode;


	Private() :
		initialized(false),
		search_mode(Library::CaseInsensitve)
	{}
};

SearchMode::SearchMode(const QSqlDatabase& db, DbId db_id) :
	DB::Module(db, db_id)
{
	m = Pimpl::make<SearchMode::Private>();
}

SearchMode::~SearchMode() {}


void SearchMode::init()
{
	if(m->initialized){
		return;
	}

	AbstrSetting* s = Settings::instance()->setting(SettingKey::Lib_SearchMode);
	QString db_key = s->db_key();

	Query q(this);
	q.prepare("SELECT value FROM settings WHERE key = '" + db_key + "';");

	if(q.exec()) {
		if (q.next()) {
			m->initialized = true;
			m->search_mode = q.value(0).toInt();
		}
	}
}

Library::SearchModeMask SearchMode::search_mode()
{
	init();

	return m->search_mode;
}

void SearchMode::update_search_mode()
{
	m->initialized = false;
	init();
}
