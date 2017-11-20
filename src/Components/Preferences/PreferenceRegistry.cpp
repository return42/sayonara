#include "PreferenceRegistry.h"

#include <QList>
#include <QString>

struct PreferenceRegistry::Private
{
	QList<QString>	preferences;
	PreferenceUi*	user_interface=nullptr;
};

PreferenceRegistry::PreferenceRegistry()
{
	m = Pimpl::make<Private>();
}

PreferenceRegistry::~PreferenceRegistry() {}

void PreferenceRegistry::register_preference(const QString& name)
{
	if(!m->preferences.contains(name))
	{
		m->preferences << name;
	}
}

void PreferenceRegistry::set_user_interface(PreferenceUi* ui)
{
	m->user_interface = ui;
}

void PreferenceRegistry::show_preference(const QString& name)
{
	if(m->user_interface){
		m->user_interface->show_preference(name);
	}
}
