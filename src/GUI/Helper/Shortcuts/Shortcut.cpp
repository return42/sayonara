#include "Shortcut.h"
#include "ShortcutHandler.h"
#include "GUI/Helper/SayonaraWidget.h"


Shortcut::Shortcut(SayonaraShortcutWidget* parent, const QString& identifier, const QString& name, const QStringList& default_shortcuts) :
	SayonaraClass()
{
	_name = name;
	_identifier = identifier;
	_parent=parent;

	_default_shortcuts = default_shortcuts;
	for(QString& str : _default_shortcuts){
		str.replace(" +", "+");
		str.replace("+ ", "+");
	}

	_shortcuts = _default_shortcuts;

	RawShortcutMap rsm = _settings->get(Set::Player_Shortcuts);

	if(rsm.contains(identifier)){
		_shortcuts = rsm[identifier];
	}

	else{
		_shortcuts = _default_shortcuts;
	}
}

Shortcut::Shortcut(SayonaraShortcutWidget* parent, const QString& identifier, const QString& name, const QString& default_shortcut) :
	Shortcut(parent, identifier, name, QStringList(default_shortcut))
{

}

Shortcut::Shortcut(const Shortcut& other)
{
	_parent =				other._parent;
	_name =					other._name;
	_identifier =			other._identifier;
	_default_shortcuts =	other._default_shortcuts;
	_shortcuts =			other._shortcuts;
	_qt_shortcuts =			other._qt_shortcuts;
}

Shortcut::Shortcut(){

}

QString Shortcut::get_name() const
{
	if(_parent){
		QString name = _parent->get_shortcut_text(_identifier);
		if(!name.isEmpty()){
			return name;
		}
	}
	return _name;
}

QStringList Shortcut::get_default() const
{
	return _default_shortcuts;
}

QList<QKeySequence> Shortcut::get_sequences() const
{
	QList<QKeySequence> sequences;
	for(const QString& str : get_shortcuts()){
		QKeySequence seq = QKeySequence::fromString(str, QKeySequence::NativeText);
		sequences << seq;
	}

	return sequences;
}

QStringList Shortcut::get_shortcuts() const
{
	return _shortcuts;
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



void Shortcut::create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot)
{
	QList<QShortcut*> shortcuts = init_qt_shortcut(parent);
	for(QShortcut* sc : shortcuts){
		parent->connect(sc, SIGNAL(activated()), receiver, slot);
	}
}


QList<QShortcut*> Shortcut::init_qt_shortcut(QWidget* parent)
{

	QList<QShortcut*> lst;
	if(get_sequences().size() > 1){
		sp_log(Log::Debug) << "Number of shortcuts: " << get_sequences().size();
	}

	for(const QKeySequence& sequence : get_sequences()){
		QShortcut* shortcut = new QShortcut(parent);

		shortcut->setContext(Qt::WindowShortcut);
		shortcut->setKey(sequence);

		_qt_shortcuts << shortcut;

		lst << shortcut;
	}

	ShortcutHandler::getInstance()->set_shortcut(*this);

	return lst;
}


void Shortcut::change_shortcut(const QStringList &shortcuts){

	_shortcuts = shortcuts;
	for(QString& str : _shortcuts){
		str.replace(" +", "+");
		str.replace("+ ", "+");
	}

	for(QShortcut* sc : _qt_shortcuts){
		QList<QKeySequence> sequences = get_sequences();
		for(const QKeySequence& ks : sequences){
			sc->setKey(ks);
		}
	}

	ShortcutHandler::getInstance()->set_shortcut(*this);
}
