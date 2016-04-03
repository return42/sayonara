#include "GUI_PlayerPreferences.h"


GUI_PlayerPreferences::GUI_PlayerPreferences(QWidget *parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_PlayerPreferences()
{

}

GUI_PlayerPreferences::~GUI_PlayerPreferences()
{

}



void GUI_PlayerPreferences::init_ui()
{
	setup_parent(this);

	revert();
}

QString GUI_PlayerPreferences::get_action_name() const
{
	return tr("Player");
}

QLabel*GUI_PlayerPreferences::get_title_label()
{
	return lab_title;
}

void GUI_PlayerPreferences::commit()
{
	_settings->set(Set::Player_Min2Tray, cb_close_to_tray->isChecked());
	_settings->set(Set::Player_StartInTray, cb_start_in_tray->isChecked());
	_settings->set(Set::Player_NotifyNewVersion, cb_update_notifications->isChecked());
}

void GUI_PlayerPreferences::revert()
{
	cb_start_in_tray->setChecked(_settings->get(Set::Player_StartInTray));
	cb_close_to_tray->setChecked(_settings->get(Set::Player_Min2Tray));
	cb_update_notifications->setChecked(_settings->get(Set::Player_NotifyNewVersion));
}

void GUI_PlayerPreferences::language_changed(){

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}
