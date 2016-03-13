#include "GUI_LibraryPreferences.h"

GUI_LibraryPreferences::GUI_LibraryPreferences(QWidget* parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_LibraryPreferences()
{
}

void GUI_LibraryPreferences::init_ui(){

	setup_parent(this);

	rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));

	connect(btn_ok, &QRadioButton::clicked, this, &GUI_LibraryPreferences::ok_clicked);
	connect(btn_cancel, &QRadioButton::clicked, this, &GUI_LibraryPreferences::cancel_clicked);
}

QString GUI_LibraryPreferences::get_action_name() const
{
	return tr("Library-Playlist Interaction");
}

QLabel* GUI_LibraryPreferences::get_title_label()
{
	return lab_title;
}

void GUI_LibraryPreferences::ok_clicked(){
	_settings->set(Set::Lib_DC_DoNothing, rb_dc_do_nothing->isChecked());
	_settings->set(Set::Lib_DC_PlayIfStopped, rb_dc_play_if_stopped->isChecked());
	_settings->set(Set::Lib_DC_PlayImmediately, rb_dc_play_immediately->isChecked());
	_settings->set(Set::Lib_DD_DoNothing, rb_dd_do_nothing->isChecked());
	_settings->set(Set::Lib_DD_PlayIfStoppedAndEmpty, rb_dd_start_if_stopped_and_empty->isChecked());

	close();
}

void GUI_LibraryPreferences::cancel_clicked(){

	rb_dc_do_nothing->setChecked(_settings->get(Set::Lib_DC_DoNothing));
	rb_dc_play_if_stopped->setChecked(_settings->get(Set::Lib_DC_PlayIfStopped));
	rb_dc_play_immediately->setChecked(_settings->get(Set::Lib_DC_PlayImmediately));
	rb_dd_do_nothing->setChecked(_settings->get(Set::Lib_DD_DoNothing));
	rb_dd_start_if_stopped_and_empty->setChecked(_settings->get(Set::Lib_DD_PlayIfStoppedAndEmpty));

	close();
}

void GUI_LibraryPreferences::language_changed()
{
	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceDialogInterface::language_changed();
}
