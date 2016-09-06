#include "LocalLibraryContainer.h"
#include "GUI/Library/GUI_LocalLibrary.h"
#include "GUI/Helper/GUI_Helper.h"

LocalLibraryContainer::LocalLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{

}

QString LocalLibraryContainer::get_name() const
{
	return "local_library";
}

QString LocalLibraryContainer::get_display_name() const {
	return tr("Local Library");
}

QIcon LocalLibraryContainer::get_icon() const {
	return GUI::get_icon("append");
}

QWidget* LocalLibraryContainer::get_ui() const {
	return static_cast<QWidget*>(_ui);
}

QComboBox* LocalLibraryContainer::get_libchooser(){
	return _ui->get_libchooser();
}

QMenu*LocalLibraryContainer::get_menu()
{
	if(_ui){
		return _ui->get_menu();
	}

	return nullptr;
}

void LocalLibraryContainer::init_ui()
{
	_ui = new GUI_LocalLibrary();
}
