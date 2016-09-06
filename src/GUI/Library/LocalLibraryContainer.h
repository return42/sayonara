#ifndef LOCALLIBRARYCONTAINER_H
#define LOCALLIBRARYCONTAINER_H

#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

class GUI_LocalLibrary;

class LocalLibraryContainer :
	public LibraryContainerInterface
{
	Q_OBJECT

private:
	GUI_LocalLibrary*   _ui=nullptr;

public:

	LocalLibraryContainer(QObject* parent=nullptr);

	// override from LibraryViewInterface
	QString     get_name() const override;
	QString     get_display_name() const override;
	QIcon       get_icon() const override;
	QWidget*    get_ui() const override;
	QComboBox*  get_libchooser() override;
	QMenu*      get_menu() override;
	void        init_ui() override;
};


#endif // LOCALLIBRARYCONTAINER_H
