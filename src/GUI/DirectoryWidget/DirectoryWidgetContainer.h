#ifndef DIRECTORYWIDGETCONTAINER_H
#define DIRECTORYWIDGETCONTAINER_H

#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

class GUI_DirectoryWidget;

// for showing up in library tree
class DirectoryLibraryContainer :
		public LibraryContainerInterface
{

	Q_OBJECT

private:
	GUI_DirectoryWidget*		ui=nullptr;

public:

	DirectoryLibraryContainer(QObject* parent=nullptr);
	virtual ~DirectoryLibraryContainer();

	QIcon				get_icon() const override;
	QString				get_display_name() const override;
	QString				get_name() const override;
	QWidget*			get_ui() const override;
	QComboBox*			get_libchooser() override;
	void				init_ui() override;
};

#endif // DIRECTORYWIDGETCONTAINER_H
