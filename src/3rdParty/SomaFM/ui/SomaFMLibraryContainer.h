#ifndef GUI_SOMAFMLIBRARY_CONTAINER_H
#define GUI_SOMAFMLIBRARY_CONTAINER_H

#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include <QtGlobal>

class GUI_SomaFM;

class SomaFMLibraryContainer :
	public LibraryContainerInterface
{

	Q_OBJECT

#ifdef Q_OS_WIN

#undef Q_PLUGIN_METADATA
#undef Q_INTERFACES
#undef IID

#else

/*	Q_PLUGIN_METADATA(IID "com.sayonara-player.somafm_library")
	Q_INTERFACES(LibraryContainerInterface)*/

#endif


private:
	GUI_SomaFM*	ui=nullptr;


public:

	SomaFMLibraryContainer(QObject* parent=nullptr);

	// override from LibraryViewInterface
	QString			get_name() const override;
	QString			get_display_name() const override;
	QIcon			get_icon() const override;
	QWidget*		get_ui() const override;
	QComboBox*		get_libchooser() override;
	QMenu*			get_menu() override;
	void			init_ui() override;

};

#endif