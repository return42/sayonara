#include "ShortcutHandler.h"
#include "Helper/Settings/Settings.h"

#include <functional>

Shortcut::Shortcut(const QString& identifier, const QString& name, const QString& default_shortcut) :
	SayonaraClass()
{
	_name = name;
	_identifier = identifier;

	_default_shortcut = default_shortcut;
	_default_shortcut.replace(" ", "");
	_shortcut = _default_shortcut;

	RawShortcutMap rsm = _settings->get(Set::Player_Shortcuts);

	if(rsm.keys().contains(identifier)){
		_shortcut = rsm[identifier];
	}

	else{
		_shortcut = _default_shortcut;
	}
}

Shortcut::Shortcut(const Shortcut& other)
{
	_name =				other._name;
	_identifier =		other._identifier;
	_default_shortcut = other._default_shortcut;
	_shortcut =			other._shortcut;
	_qt_shortcuts =		other._qt_shortcuts;
}

Shortcut::Shortcut(){

}

QString Shortcut::get_name() const
{
	return _name;
}

QString Shortcut::get_default() const
{
	return _default_shortcut;
}

QKeySequence Shortcut::get_sequence() const
{
	return QKeySequence(get_shortcut());
}

QString Shortcut::get_shortcut() const
{
	return _shortcut;
}

QString Shortcut::get_identifier() const
{
	return _identifier;
}

Shortcut Shortcut::getInvalid(){
	return Shortcut();
}

bool Shortcut::is_valid() const
{
	return !(_identifier.isEmpty());
}

QString Shortcut::to_string() const
{
	return _identifier + ": " + _shortcut;
}


void Shortcut::create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot)
{
	QShortcut* shortcut = init_qt_shortcut(parent);

	parent->connect(shortcut, SIGNAL(activated()), receiver, slot);
}


QShortcut* Shortcut::init_qt_shortcut(QWidget* parent)
{
	QShortcut* shortcut = new QShortcut(parent);

	shortcut->setContext(Qt::WindowShortcut);
	shortcut->setKey(this->get_sequence());

	_qt_shortcuts << shortcut;

	ShortcutHandler::getInstance()->set_shortcut(*this);

	return shortcut;
}


void Shortcut::change_shortcut(const QString &shortcut){

	_shortcut = shortcut;
	_shortcut.replace(" ", "");

	for(QShortcut* sc : _qt_shortcuts){
		sc->setKey(this->get_sequence());
	}

	ShortcutHandler::getInstance()->set_shortcut(*this);
}

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

void ShortcutHandler::set_shortcut(const QString& identifier, const QString& shortcut)
{
	RawShortcutMap rsm;
	for(auto it = _shortcuts.begin(); it != _shortcuts.end(); it++)
	{
		if(it->get_identifier() == identifier){
			it->change_shortcut(shortcut);
		}

		rsm[it->get_identifier()] = it->get_shortcut();
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

