#include "Helper/Settings/Settings.h"
#include "Helper/Settings/SettingKey.h"
#include "Helper/globals.h"

class SettingRegistry
{
	SINGLETON(SettingRegistry)

private:
	Settings* 	_settings=nullptr;

	template<typename KEY, typename T>
	void register_setting(const KEY& key, const char* db_key, const T& default_value){

		typedef decltype(key.p) ValueTypePtr;
		typedef typename std::remove_pointer<ValueTypePtr>::type ValueType;
		auto setting = new Setting<ValueType>(key, db_key, default_value);

		_settings->register_setting( setting );
	} 

	template<typename KEY, typename T>
	void register_setting(const KEY& key, const T& default_value){

		typedef decltype(key.p) ValueTypePtr;
		typedef typename std::remove_pointer<ValueTypePtr>::type ValueType;
		auto setting = new Setting<ValueType>(key, default_value);

		_settings->register_setting( setting );
	}

public:
	bool init();
};

