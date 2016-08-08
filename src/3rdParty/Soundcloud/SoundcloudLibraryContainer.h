/* SoundcloudLibraryContainer.h */

#ifndef SOUNDCLOUD_LIBRARY_CONTAINER
#define SOUNDCLOUD_LIBRARY_CONTAINER

#include <QtGlobal>
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

class GUI_SoundCloudLibrary;

class SoundcloudLibraryContainer :
	public LibraryContainerInterface
{

	Q_OBJECT

	Q_PLUGIN_METADATA(IID "com.sayonara-player.soundcloud_library")
	Q_INTERFACES(LibraryContainerInterface)
	

private:
	GUI_SoundCloudLibrary*	ui=nullptr;

public:

	SoundcloudLibraryContainer(QObject* parent=nullptr);

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
