#include "SoundcloudLibraryContainer.h"
#include "ui/GUI_SoundcloudLibrary.h"
#include "src/SoundcloudLibrary.h"


SoundcloudLibraryContainer::SoundcloudLibraryContainer(QObject *parent) :
	LibraryContainerInterface(parent)
{
	Q_INIT_RESOURCE(SoundcloudIcons);
}

QString SoundcloudLibraryContainer::get_name() const
{
	return "soundcloud";
}

QString SoundcloudLibraryContainer::get_display_name() const
{
	return tr("Soundcloud");
}

QIcon SoundcloudLibraryContainer::get_icon() const
{
	return QIcon(":/sc_icons/ui/icon.png");
}


QWidget* SoundcloudLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(ui);
}

QComboBox* SoundcloudLibraryContainer::get_libchooser()
{
	if(ui){
		return ui->get_libchooser();
	}

	return nullptr;
}

QMenu* SoundcloudLibraryContainer::get_menu()
{
	if(ui){
		return ui->get_menu();
	}

	return nullptr;
}

void SoundcloudLibraryContainer::init_ui()
{
	SoundcloudLibrary* library = new SoundcloudLibrary(this);
	ui = new GUI_SoundCloudLibrary(library);
}

