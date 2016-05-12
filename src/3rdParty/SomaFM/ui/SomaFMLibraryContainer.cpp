#include "ui/SomaFMLibraryContainer.h"
#include "ui/GUI_SomaFM.h"


SomaFMLibraryContainer::SomaFMLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{
	Q_INIT_RESOURCE(SomaFMIcons);
}

QString SomaFMLibraryContainer::get_name() const
{
	return "SomaFM";
}

QString SomaFMLibraryContainer::get_display_name() const
{
	return "SomaFM";
}

QIcon SomaFMLibraryContainer::get_icon() const
{
	return QIcon(":/soma_icons/soma.png");
}

QWidget* SomaFMLibraryContainer::get_ui() const
{
	return ui;
}

QComboBox* SomaFMLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

QMenu* SomaFMLibraryContainer::get_menu()
{
	return nullptr;
}

void SomaFMLibraryContainer::init_ui()
{
	ui = new GUI_SomaFM(nullptr);
}

