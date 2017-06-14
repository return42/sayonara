#ifndef LIBRARYPLUGINMENU_H
#define LIBRARYPLUGINMENU_H

#include <QMenu>
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"

class LibraryPluginMenu :
        public QMenu,
        public SayonaraClass
{
	Q_OBJECT
	PIMPL(LibraryPluginMenu)

public:
	LibraryPluginMenu(QWidget* parent=nullptr);
	~LibraryPluginMenu();

public slots:
	void setup_actions();

private slots:
	void action_triggered(bool b);
	void current_library_changed(const QString& name);
};

#endif // LIBRARYPLUGINMENU_H
