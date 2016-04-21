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

/**
 * @brief A singleton class for retrieving shortcuts
 * @ingroup Shortcuts
 */
class ShortcutHandler : public SayonaraClass
{

	SINGLETON(ShortcutHandler)

public:


private:
	QList<Shortcut> _shortcuts;


public:

	/**
	 * @brief get a shortcut by its unique identifier
	 * @param identifier the identifier which is used in database
	 * @return a shortcut instance
	 */
	Shortcut get_shortcut(const QString& identifier) const;

	/**
	 * @brief set the shortcut by its unique identifier
	 * @param identifier  the identifier which is used in database
	 * @param shortcut a shortcut instance
	 */
	void set_shortcut(const QString& identifier, const QStringList& shortcut);

	/**
	 * @brief Overwrite a shortcut
	 * @param shortcut a shortcut instance
	 */
	void set_shortcut(const Shortcut& shortcut);

	/**
	 * @brief add a new shortcut instance to the handler. This is usually done
	 * by the widget the shortcut is attached to. So you can use the same shortcut
	 * on multiple widgets
	 * @param shortcut a shortcut instance
	 * @return an invalid shortcut, if source shortcut is invalid, too\n
	 * if the shortcut already exists, the instance already known is returned\n
	 * if the shortcut does not exist yet, the same shortcut as the input is returned
	 */
	Shortcut add(const Shortcut& shortcut);


	/**
	 * @brief get all shortcuts
	 * @return
	 */
	QList<Shortcut>	get_shortcuts() const;

};

#endif // SHORTCUTHANDLER_H
