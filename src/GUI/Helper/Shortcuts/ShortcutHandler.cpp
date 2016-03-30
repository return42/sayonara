#include "ShortcutHandler.h"
#include "Helper/Settings/Settings.h"

#include <functional>


ShortcutHandler::ShortcutHandler() :
	SayonaraClass()
{


}

ShortcutHandler::~ShortcutHandler(){

}

Shortcut ShortcutHandler::get_shortcut(const QString& identifier) const
{
	for(auto it = _shortcuts.begin(); it != _shortcuts.end(); it++)
	{
		if(it->get_identifier() == identifier){
			return *it;
		}
	}

	return Shortcut::getInvalid();
}

void ShortcutHandler::set_shortcut(const QString& identifier, const QStringList& shortcuts)
{
	RawShortcutMap rsm;
	for(auto it = _shortcuts.begin(); it != _shortcuts.end(); it++)
	{
		if(it->get_identifier() == identifier){
			it->change_shortcut(shortcuts);
		}

		rsm[it->get_identifier()] = it->get_shortcuts();
	}

	_settings->set(Set::Player_Shortcuts, rsm);
}

void ShortcutHandler::set_shortcut(const Shortcut& shortcut)
{
	for(auto it = _shortcuts.begin(); it != _shortcuts.end(); it++)
	{
		if(it->get_identifier() == shortcut.get_identifier()){
			*it = shortcut;
		}
	}
}

Shortcut ShortcutHandler::add(const Shortcut& shortcut)
{
	if(!shortcut.is_valid()){
		Shortcut::getInvalid();
	}

	Shortcut sc = get_shortcut(shortcut.get_identifier());
	if(sc.is_valid()){
		return sc;
	}

	_shortcuts << shortcut;
	return shortcut;
}

QList<Shortcut> ShortcutHandler::get_shortcuts() const
{
	return _shortcuts;
}

