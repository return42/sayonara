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

#pragma once

template<typename KeyClass>
class SettingNotifier
{
private:
	std::map<int, std::function<void ()>> _callbacks;
	int _idx;

	SettingNotifier() : _idx(0) {}
	SettingNotifier(const SettingNotifier& other) { (void) (other); }


	void add_listener(std::function<void ()> fn)
	{
		_callbacks[_idx] = fn;
		_idx++;
	}

public:
	~SettingNotifier() {}

	static SettingNotifier<KeyClass>* instance()
	{
		static SettingNotifier<KeyClass> inst;
		return &inst;
	}

	static void reg(std::function<void ()> fn)
	{
		instance()->add_listener(fn);
	}

	void val_changed()
	{
		for(auto v : _callbacks) {
			v.second ();
		}
	}
};


namespace Set
{
	template<typename T, typename KeyClassInstance>
	void listen(const KeyClassInstance& key, T* t, void (T::*fn)(), bool run=true)
	{
		using KeyClass=decltype(key);
		auto callable = std::bind(fn, t);
		SettingNotifier<KeyClass>::reg( callable );

		if(run){
			callable();
		}
	}

	template<typename KeyClassInstance>
	void listen(const KeyClassInstance& key, std::function<void ()> fn, bool run=true)
	{
		using KeyClass=decltype(key);
		//auto callable = std::bind(l, t);
		SettingNotifier<KeyClass>::reg( fn );

		if(run){
			fn();
		}
	}
}


#endif // SETTINGNOTIFIER_H
