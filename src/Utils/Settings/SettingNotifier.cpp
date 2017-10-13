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
#include <QMap>
#include <iostream>

#include <mutex>

struct NotifyClassRegistry::Private
{
    QMap<SayonaraClass*, bool> registered_classes;
};

NotifyClassRegistry::NotifyClassRegistry()
{
    m = Pimpl::make<Private>();
}

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
    return (m->registered_classes.keys().contains(c));
}


void NotifyClassRegistry::add_class(SayonaraClass *c)
{
    m->registered_classes.insert(c, true);
}

void NotifyClassRegistry::remove_class(SayonaraClass *c, AbstrSettingNotifier* asn)
{
    bool has_class = (m->registered_classes.keys().contains(c));
    if(has_class)
    {
        if(!m->registered_classes[c]){
            sp_log(Log::Debug, this) << "try to remove class 2 times";
        }

        if(asn && m->registered_classes[c]){
            asn->class_removed(c);
        }

        m->registered_classes[c] = false;
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
