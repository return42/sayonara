#ifndef LIBRARYPLUGINCOMBOBOX_H
#define LIBRARYPLUGINCOMBOBOX_H

#include <QComboBox>
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"
#include "GUI/Helper/SayonaraWidget/SayonaraComboBox.h"

class QEvent;

class LibraryPluginCombobox :
		public SayonaraComboBox
{
	Q_OBJECT
	PIMPL(LibraryPluginCombobox)

public:
	explicit LibraryPluginCombobox(const QString& text, QWidget* parent=nullptr);
	~LibraryPluginCombobox();

public slots:
	void setup_actions();

private slots:
	void action_triggered(bool b);
	void current_library_changed(const QString& name);
};



#endif // LIBRARYPLUGINCOMBOBOX_H
