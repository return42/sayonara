#ifndef LIBRARYPLUGINCOMBOBOX_H
#define LIBRARYPLUGINCOMBOBOX_H

#include <QComboBox>
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"

class QEvent;

class LibraryPluginCombobox :
		public QComboBox,
		public SayonaraClass
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
	void changeEvent(QEvent* event) override;
	void style_changed();
};



#endif // LIBRARYPLUGINCOMBOBOX_H
