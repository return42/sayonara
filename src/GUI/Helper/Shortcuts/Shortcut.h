#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QString>
#include <QWidget>
#include <QStringList>
#include <QShortcut>
#include <QKeySequence>
#include "Helper/SayonaraClass.h"

class Shortcut : private SayonaraClass
{

private:
	QStringList			_default_shortcuts;
	QStringList			_shortcuts;
	QString				_name;
	QString				_identifier;
	QList<QShortcut*>	_qt_shortcuts;

private:
	Shortcut();
	QList<QShortcut*> init_qt_shortcut(QWidget* parent);


public:
	Shortcut(const QString& identifier, const QString& name, const QString& default_shortcut);
	Shortcut(const QString& identifier, const QString& name, const QStringList& default_shortcuts);
	Shortcut(const Shortcut& other);

	static Shortcut getInvalid();

	void change_shortcut(const QStringList& shortcuts);
	QString get_name() const;
	QStringList get_default() const;
	QList<QKeySequence> get_sequences() const;
	QStringList get_shortcuts() const;
	QString get_identifier() const;
	bool is_valid() const;

	template<typename T>
	void create_qt_shortcut(QWidget* parent, T func){

		QList<QShortcut*> shortcuts = init_qt_shortcut(parent);
		for(QShortcut* sc : shortcuts){
			parent->connect(sc, &QShortcut::activated, func);
		}
	}

	void create_qt_shortcut(QWidget* parent, QObject* receiver, const char* slot);

};


#endif // SHORTCUT_H
