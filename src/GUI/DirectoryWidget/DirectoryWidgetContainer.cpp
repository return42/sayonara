#include "DirectoryWidgetContainer.h"

#include "GUI_DirectoryWidget.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

DirectoryLibraryContainer::DirectoryLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{

}

DirectoryLibraryContainer::~DirectoryLibraryContainer() {}

QString DirectoryLibraryContainer::get_name() const
{
	return "directories";
}

QString DirectoryLibraryContainer::get_display_name() const
{
	return tr("Directories");
}

QIcon DirectoryLibraryContainer::get_icon() const
{
	return IconLoader::getInstance()->get_icon("folder", "folder");
}

QWidget* DirectoryLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(ui);
}

QComboBox*DirectoryLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

void DirectoryLibraryContainer::init_ui()
{
	ui = new GUI_DirectoryWidget(nullptr);
}
