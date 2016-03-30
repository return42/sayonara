#include "RawShortcutMap.h"
#include <QStringList>

QString RawShortcutMap::toString() const {

	QStringList entries;

	for(const QString& key : this->keys()){
		QString shortcut_name = key;
		QStringList shortcuts = this->value(key);

		entries << shortcut_name + ":" + shortcuts.join(", ");
	}

	return entries.join("---");
}

RawShortcutMap RawShortcutMap::fromString(const QString& setting){

	RawShortcutMap rsc;

	QStringList entries = setting.split("---");
	for(const QString& entry : entries){

		QStringList sc_pair = entry.split(":");
		QString key = sc_pair[0];
		QString shortcut;
		if(sc_pair.size() > 1){
			shortcut = sc_pair[1];
		}

		rsc.insert(key, shortcut.split(", "));
	}

	return rsc;
}
