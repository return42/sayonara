#ifndef SHORTCUTHANDLER_H
#define SHORTCUTHANDLER_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QKeySequence>
#include <QShortcut>

#include "Helper/globals.h"
#include "RawShortcutMap.h"
#include "Shortcut.h"
#include "Helper/SayonaraClass.h"

#include <functional>


class ShortcutHandler : public SayonaraClass
{

	SINGLETON(ShortcutHandler)

public:


private:
	QList<Shortcut> _shortcuts;


public:

	Shortcut get_shortcut(const QString& identifier) const;
	void set_shortcut(const QString& identifier, const QStringList& shortcut);
	void set_shortcut(const Shortcut& shortcut);

	Shortcut add(const Shortcut& shortcut);

	QList<Shortcut>	get_shortcuts() const;

};

#endif // SHORTCUTHANDLER_H
