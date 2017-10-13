/* SettingNotifier.cpp */

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

#include "Utils/Settings/SettingNotifier.h"
#include "Utils/Settings/SettingKey.h"
#include "Utils/Utils.h"
#include "Utils/Set.h"
#include <iostream>

#include <mutex>

NotifyClassRegistry::NotifyClassRegistry() {}

NotifyClassRegistry::~NotifyClassRegistry() {}

static NotifyClassRegistry* x=nullptr;
NotifyClassRegistry* NotifyClassRegistry::instance()
{
	if(!x){
		x = new NotifyClassRegistry();
	}

	return x;
}

void NotifyClassRegistry::destroy()
{
	delete x; x=nullptr;
}

bool NotifyClassRegistry::check_class(SayonaraClass *c)
{
    return (registered_classes.find(c) != registered_classes.end());
}


void NotifyClassRegistry::add_class(SayonaraClass *c)
{
    registered_classes.insert(c);
}

void NotifyClassRegistry::remove_class(SayonaraClass *c, AbstrSettingNotifier* asn)
{

    auto it = registered_classes.find(c);
    if(it != registered_classes.end())
    {
        registered_classes.erase(it);

        if(asn){
            asn->class_removed(c);
        }
    }
	else {
		SP::Set<int64_t> classes;
		for(SayonaraClass* x : registered_classes){
			classes.insert((int64_t) x);
		}
	}
}


void Set::class_destroyed(SayonaraClass *t)
{
    NotifyClassRegistry::instance()->remove_class(t);
}

void Set::register_class(SayonaraClass* t)
{
	NotifyClassRegistry::instance()->add_class(t);
}
