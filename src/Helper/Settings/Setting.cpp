/* Setting.cpp */

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

#include "Setting.h"
#include "Helper/Logger/Logger.h"
#include "Database/DatabaseSettings.h"

#include <QVariant>

struct AbstrSetting::Private
{
	SK::SettingKey	key;
	QString			db_key;
	bool			db_setting;
};


AbstrSetting::AbstrSetting()
{
	_m = Pimpl::make<Private>();
}

AbstrSetting::AbstrSetting(SK::SettingKey key) :
	AbstrSetting()
{
	_m->key = key;
	_m->db_setting = false;
}

AbstrSetting::AbstrSetting(SK::SettingKey key, const char* db_key) :
	AbstrSetting(key)
{
	_m->db_key = db_key;
	_m->db_setting = true;
}


AbstrSetting::AbstrSetting(const AbstrSetting& other) :
	AbstrSetting()
{
	_m->key = other._m->key;
	_m->db_key = other._m->db_key;
	_m->db_setting = other._m->db_setting;
}

AbstrSetting& AbstrSetting::operator=(const AbstrSetting& other)
{
	_m->key = other._m->key;
	_m->db_key = other._m->db_key;
	_m->db_setting = other._m->db_setting;

	return *this;
}

AbstrSetting::~AbstrSetting() {}

SK::SettingKey AbstrSetting::get_key() const
{
	return _m->key;
}

void AbstrSetting::load_db(DatabaseSettings *db)
{
	if(!_m->db_setting) return;

	QString s;
	bool success = db->load_setting(_m->db_key, s);

	if(!success){
		sp_log(Log::Info) << "Setting " << _m->db_key << ": Not found. Use default value...";

		assign_default_value();

		sp_log(Log::Info) << "Load Setting " << _m->db_key << ": " << value_to_string();

		return;
	}

	success = load_value_from_string(s);

	if(!success){
		sp_log(Log::Warning) << "Setting " << _m->db_key << ": Cannot convert. Use default value...";

		assign_default_value();
	}
}

void AbstrSetting::store_db(DatabaseSettings* db)
{
	if(_m->db_setting){
		QString val_as_string = value_to_string();
		db->store_setting(_m->db_key, val_as_string);
	}
}



