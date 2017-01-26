/* Settings.h */

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
#pragma once
#ifndef SAYONARA_SETTINGS_H_
#define SAYONARA_SETTINGS_H_


#include "Helper/Settings/Setting.h"
#include "Helper/Settings/SettingNotifier.h"
#include "Helper/Singleton.h"


/**
 * @brief The Settings class
 * @ingroup Settings
 */
class Settings : public QObject
{
	SINGLETON(Settings)
	PIMPL(Settings)

private:
	AbstrSetting* setting(SK::SettingKey key) const;

public:

	/* get all settings (used by database) */
	AbstrSetting**	get_settings();


	/* before you want to access a setting you have to register it */
	void register_setting(AbstrSetting* s);


	/* checks if all settings are registered */
	bool check_settings();


	/* get a setting, defined by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	const T& get(const SettingKey<T,S>& k) const
	{
		Q_UNUSED(k);
		Setting<T>* s = (Setting<T>*) setting(S);
		return s->value();
	}

	/* set a setting, define by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	void set(const SettingKey<T,S>& key, const T& val)
	{
		Q_UNUSED(key)
		Setting<T>* s = (Setting<T>*) setting(S);

		if( s->set_value(val)) {
			SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
			sn->val_changed();
		}
	}

	/* get a setting, defined by a unique, REGISTERED key */
	template<typename T, SK::SettingKey S>
	void shout(const SettingKey<T,S>& k) const
	{
		Q_UNUSED(k);
		SettingNotifier< SettingKey<T, S> >* sn = SettingNotifier< SettingKey<T, S> >::getInstance();
		sn->val_changed();
	}
};

#endif // SAYONARA_SETTINGS_H_
