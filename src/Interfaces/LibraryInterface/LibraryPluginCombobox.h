#ifndef LIBRARYPLUGINCOMBOBOX_H
#define LIBRARYPLUGINCOMBOBOX_H

#include <QComboBox>
#include "Utils/Pimpl.h"
#include "Utils/Settings/SayonaraClass.h"
#include "GUI/Utils/Widgets/ComboBox.h"

class QEvent;

class LibraryPluginCombobox :
		public Gui::ComboBox
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
