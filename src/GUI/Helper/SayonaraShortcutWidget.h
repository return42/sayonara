#ifndef SAYONARASHORTCUTWIDGET_H
#define SAYONARASHORTCUTWIDGET_H

#include <QString>

class SayonaraShortcutWidget {

public:
	virtual QString get_shortcut_text(const QString& shortcut_identifier) const=0;
};


#endif // SAYONARASHORTCUTWIDGET_H
