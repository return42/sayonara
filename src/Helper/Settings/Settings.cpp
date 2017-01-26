/* Settings.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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

#include "Helper/Logger/Logger.h"
#include "Helper/Settings/Settings.h"
#include "Helper/typedefs.h"

struct Settings::Private
{
	QString			db_file;
	QString			version;
	AbstrSetting*	settings[SK::Num_Setting_Keys + 1];

	Private()
	{
		db_file = "player.db";
		memset(settings, 0, SK::Num_Setting_Keys + 1);
	}
};

Settings::Settings()
{
	_m = Pimpl::make<Private>();
}

Settings::~Settings () {}


AbstrSetting* Settings::setting(SK::SettingKey key) const
{
	return _m->settings[(int) key];
}

AbstrSetting** Settings::get_settings()
{
	return _m->settings;
}

void Settings::register_setting(AbstrSetting* s)
{
	SK::SettingKey key  = s->get_key();
	_m->settings[ (int) key ] = s;
}


bool Settings::check_settings()
{
	IntList un_init;
	for(int i=0; i<SK::Num_Setting_Keys; i++){
		if(! _m->settings[i] ){
			un_init << i;
		}
	}

	if( !un_init.empty() ){
		sp_log(Log::Warning) << "**** Settings " << un_init << " are not initialized ****";
		return false;
	}

	else{
		sp_log(Log::Info) << "**** All settings initialized ****";
	}

	return true;
}
