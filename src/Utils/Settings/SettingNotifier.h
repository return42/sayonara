/* SettingNotifier.h */

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

#ifndef SETTINGNOTIFIER_H
#define SETTINGNOTIFIER_H

#include <functional>
#include <map>
#include <set>
#include <algorithm>
#include <type_traits>

#include <unordered_map>
#include "Utils/Logger/Logger.h"
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Settings/SettingKey.h"
#include "Utils/Singleton.h"
#include "Utils/Pimpl.h"

#pragma once

class AbstrSettingNotifier;

class NotifyClassRegistry
{
    PIMPL(NotifyClassRegistry)
	NotifyClassRegistry();

public:
	~NotifyClassRegistry();
	static NotifyClassRegistry* instance();

    void add_class(SayonaraClass* c);
    void remove_class(SayonaraClass* c, AbstrSettingNotifier* asn=nullptr);
    bool check_class(SayonaraClass *c);
	void destroy();
};

class AbstrSettingNotifier
{

public:
    virtual void class_removed(SayonaraClass* c)=0;
    virtual void all_classes_removed()=0;
};

template<typename KeyClass>
class SettingNotifier :
    public AbstrSettingNotifier
{
private:
    std::map<int, std::function<void ()>> _callbacks;
	std::unordered_multimap<SayonaraClass*, int> _class_idx_map;

    using ClassIdxIterator = std::unordered_multimap<SayonaraClass*, int>::iterator;
    using CallbackIterator = std::map<int, std::function<void ()>>::iterator;

	int _idx;

    SettingNotifier() :
        AbstrSettingNotifier(),
        _idx(0)
    {}

    SettingNotifier(const SettingNotifier& other) = delete;

    bool check_index(int idx)
    {
        NotifyClassRegistry* ncr = NotifyClassRegistry::instance();
        for(auto it : _class_idx_map)
        {
            if(it.second == idx)
            {
                if(!ncr->check_class(it.first))
                {
                    ncr->remove_class(it.first, this);
                    return false;
                }

                else {
                    return true;
                }
            }
        }

        return false;
    }

	void add_listener(SayonaraClass* c, std::function<void ()> fn)
	{
		_callbacks[_idx] = fn;
		_class_idx_map.insert( std::make_pair(c, _idx) );

		_idx++;
	}

    void class_removed(SayonaraClass* c) override
    {
        auto range = _class_idx_map.equal_range(c);

        for(ClassIdxIterator it = range.first; it != range.second; it++)
        {
            int idx = it->second;

            CallbackIterator it_callback = _callbacks.find(idx);
            if(it_callback != _callbacks.end())
            {
                _callbacks.erase(it_callback);
                sp_log(Log::Warning, this) << "Removed class";
                sp_log(Log::Warning, this) << "Erase listener";
            }

            else {
                sp_log(Log::Warning, this) << "Could not find callback";
            }
        }
    }

    void all_classes_removed() override
    {
        _callbacks.clear();
        _class_idx_map.clear();
    }

public:
	~SettingNotifier() {}

	static SettingNotifier<KeyClass>* instance()
	{
		static SettingNotifier<KeyClass> inst;
		return &inst;
	}

	static void reg(SayonaraClass* c, std::function<void ()> fn)
	{
		instance()->add_listener(c, fn);
	}

	static void unreg(SayonaraClass* c)
	{
		instance()->remove_listener(c);
	}

	void val_changed()
	{
		for(auto v : _callbacks)
		{
            int idx = v.first;

            if(!check_index(idx)) {
                continue;
            }

			v.second ();
		}
	}
};



namespace Set
{
	template<typename T, typename KeyClassInstance>
	typename std::enable_if<std::is_base_of<SayonaraClass, T>::value, void>::type
	listen(const KeyClassInstance& key, T* t, void (T::*fn)(), bool run=true)
	{
		//NotifyClassRegistry* ncr = NotifyClassRegistry::instance();

		using KeyClass=decltype(key);
		auto callable = std::bind(fn, t);
		SettingNotifier<KeyClass>::reg( t, callable );

		if(run){
			callable();
		}

		//ncr->add_class(t);
	}

	template<typename KeyClassInstance>
	void listen(const KeyClassInstance& key, SayonaraClass* t, std::function<void ()> fn, bool run=true)
	{
		//NotifyClassRegistry* ncr = NotifyClassRegistry::instance();

		using KeyClass=decltype(key);
		SettingNotifier<KeyClass>::reg( t, fn );

		if(run){
			fn();
		}

		//ncr->add_class(t);
	}

	template<typename KeyClassInstance>
	void unlisten(KeyClassInstance& key, SayonaraClass* t)
	{
		//NotifyClassRegistry* ncr = NotifyClassRegistry::instance();

		using KeyClass=decltype(key);
		SettingNotifier<KeyClass>::unreg(t);

		//ncr->add_class(t);
	}

    void unregister_class(SayonaraClass* t);
	void register_class(SayonaraClass* t);
}


#endif // SETTINGNOTIFIER_H
