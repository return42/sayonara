/* Setting.h */

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

#pragma once
#ifndef SAYONARA_SETTING_H_
#define SAYONARA_SETTING_H_


#include "Helper/Settings/SettingKey.h"
#include "Helper/Settings/SettingConverter.h"
#include "Helper/Pimpl.h"

class DatabaseSettings;

/**
 * @brief The AbstrSetting class\n
 * Every setting needs a key and a value
 * The SK::SettingKey is only used inside the setting mechanism
 * @ingroup Settings
 */
class AbstrSetting
{
	PIMPL(AbstrSetting)

	private:
		AbstrSetting();
		AbstrSetting(const AbstrSetting& other);
		AbstrSetting& operator=(const AbstrSetting& other);

	protected:
		AbstrSetting(SK::SettingKey key);
		AbstrSetting(SK::SettingKey key, const char* db_key);

	public:
		virtual ~AbstrSetting();

		SK::SettingKey get_key() const;

		/* Pure virtual function for DB load/save */
		void load_db(DatabaseSettings* db);
		void store_db(DatabaseSettings* db);

		virtual bool load_value_from_string(const QString& str)=0;
		virtual QString value_to_string() const=0;
		virtual void assign_default_value()=0;
};


template< typename T,
		 template <typename Arg> class SC = SettingConverter >
/**
 * @brief The Setting class\n
 * T is the pure value type e.g. QString
 * @ingroup Settings
 */
class Setting : public AbstrSetting
{
	private:
		Setting();
		Setting(const Setting&);

		T _val;
		T _default_val;

	public:

		/* Constructor */
		template<typename SK::SettingKey S>
		Setting(const SettingKey<T, S>& key, const char* db_key, T def) :
			AbstrSetting(S, db_key)
		{
			Q_UNUSED(key);
			_default_val = def;
			_val = def;
		}

        template<typename SK::SettingKey S>
		Setting(const SettingKey<T, S>& key, T def) :
			AbstrSetting(S)
        {
            Q_UNUSED(key);
            _default_val = def;
            _val = def;
        }

		/* Destructor */
		~Setting() {}

		void assign_default_value() override
		{
			_val = _default_val;
		}

		QString value_to_string() const override
		{
			 return SC<T>::cvt_to_string(_val);
		}

		bool load_value_from_string(const QString& str) override
		{
			return SC<T>::cvt_from_string(str, _val);
		}

		/* ... */
		const T& value() const
		{
			return _val;
		}

		/* ... */
		const T& default_value() const
		{
			return _default_val;
		}

		/* ... */
		bool set_value(const T& val)
		{
			if( _val == val ){
				return false;
			}

			_val = val;
			return true;
		}
};

#endif // SAYONARA_SETTING_H_
