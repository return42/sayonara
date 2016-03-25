#ifndef SHORTCUTHANDLER_H
#define SHORTCUTHANDLER_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QKeySequence>
#include <QShortcut>

#include "Helper/globals.h"
#include "RawShortcutMap.h"
#include "Helper/SayonaraClass.h"

#include <functional>

class Shortcut : private SayonaraClass
{

private:
	QString			_default_shortcut;
	QString			_shortcut;
	QString			_name;
	QString			_identifier;
	QList<QShortcut*>		_qt_shortcuts;

private:
	Shortcut();
	QShortcut* init_qt_shortcut(QWidget* parent);


public:
	Shortcut(const QString& identifier, const QString& name, const QString& default_shortcut);
	Shortcut(const Shortcut& other);

	static Shortcut getInvalid();

	void change_shortcut(const QString& shortcut);
	QString get_name() const;
	QString get_default() const;
	QKeySequence get_sequence() const;
	QString get_shortcut() const;
	QString get_identifier() const;
	bool is_valid() const;
	QString to_string() const;

	template<typename T>
	void create_qt_shortcut(QWidget* parent, T func){

		QShortcut* shortcut = init_qt_shortcut(parent);
		parent->connect(shortcut, &QShortcut::activated, func);
	}

	void create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot);

};


class ShortcutHandler : public SayonaraClass
{

	SINGLETON(ShortcutHandler)

public:


private:
	QList<Shortcut> _shortcuts;


public:

	Shortcut get_shortcut(const QString& identifier) const;
	void set_shortcut(const QString& identifier, const QString& shortcut);
	void set_shortcut(const Shortcut& shortcut);

	Shortcut add(const Shortcut& shortcut);

	QList<Shortcut>	get_shortcuts() const;

};

#endif // SHORTCUTHANDLER_H
