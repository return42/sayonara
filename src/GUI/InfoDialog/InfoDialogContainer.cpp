#include "InfoDialogContainer.h"
#include "GUI_InfoDialog.h"
#include "GUI/Helper/GUI_Helper.h"


InfoDialogContainer::InfoDialogContainer(){

}

InfoDialogContainer::~InfoDialogContainer(){

	if(_info_dialog){
		delete _info_dialog;
		_info_dialog = nullptr;
	}
}

void InfoDialogContainer::info_dialog_closed()
{
	// we could delete the dialog here, but that's not really neccessary
}


void InfoDialogContainer::show_info(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::TabInfo);
	}
}

void InfoDialogContainer::show_lyrics(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::TabLyrics);
	}
}

void InfoDialogContainer::show_edit(){
	if(init_dialog()){
		_info_dialog->show(GUI_InfoDialog::TabEdit);
	}
}

bool InfoDialogContainer::init_dialog()
{
	if(!_info_dialog){
		_info_dialog = new GUI_InfoDialog(this, GUI::get_main_window());
	}

	_info_dialog->set_metadata(get_data_for_info_dialog(), get_metadata_interpretation());

	return _info_dialog->has_metadata();
}

