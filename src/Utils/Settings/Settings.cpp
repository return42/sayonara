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

#include "Utils/Logger/Logger.h"
#include "Utils/Settings/Settings.h"
#include "Utils/typedefs.h"

#include <array>

struct Settings::Private
{
	QString			version;
    std::array<AbstrSetting*, static_cast<int>(SettingKey::Num_Setting_Keys)> settings;

	Private()
	{
        std::fill(settings.begin(), settings.end(), nullptr);
    }
};

Settings::Settings()
{
	m = Pimpl::make<Private>();
}

Settings::~Settings () {}


AbstrSetting* Settings::setting(SettingKey key) const
{
	return m->settings[(int) key];
}

const SettingArray& Settings::settings()
{
	return m->settings;
}

void Settings::register_setting(AbstrSetting* s)
{
	SettingKey key  = s->get_key();
	m->settings[ (int) key ] = s;
}


bool Settings::check_settings()
{
	IntList un_init;

    int i=0;
    for(AbstrSetting* s : m->settings)
    {
        if(!s){
            un_init << i;
		}

        i++;
	}

    if( !un_init.empty() )
    {
        sp_log(Log::Warning, this) << "**** Settings " << un_init << " are not initialized ****";
		return false;
	}

	else{
        sp_log(Log::Info, this) << "**** All settings initialized ****";
	}

	return true;
}
