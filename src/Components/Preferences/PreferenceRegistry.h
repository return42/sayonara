#ifndef PREFERENCEREGISTRY_H
#define PREFERENCEREGISTRY_H

#include "Utils/Pimpl.h"
#include "Utils/Singleton.h"

class QString;
class PreferenceUi
{
	public:
		virtual void show_preference(const QString& name)=0;
};

class PreferenceRegistry
{
	PIMPL(PreferenceRegistry)
	SINGLETON(PreferenceRegistry)

	public:
		void register_preference(const QString& name);
		void set_user_interface(PreferenceUi* ui);
		void show_preference(const QString& name);
};

#endif // PREFERENCEREGISTRY_H
