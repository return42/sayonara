#include "GUI_StreamPreferences.h"
#include "ui_GUI_StreamPreferences.h"

#include "Utils/Settings/Settings.h"


GUI_StreamPreferences::GUI_StreamPreferences(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{}

GUI_StreamPreferences::~GUI_StreamPreferences() {}

void GUI_StreamPreferences::commit()
{
	_settings->set(Set::Stream_NewTab, ui->cb_new_tab->isChecked());
	_settings->set(Set::Stream_ShowHistory, ui->cb_show_history->isChecked());
}

void GUI_StreamPreferences::revert()
{
	ui->cb_show_history->setChecked(_settings->get(Set::Stream_ShowHistory));
	ui->cb_new_tab->setChecked(_settings->get(Set::Stream_NewTab));
}

QString GUI_StreamPreferences::get_action_name() const
{
	return tr("Streams") + " & " + tr("Podcasts");
}

void GUI_StreamPreferences::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this, &ui);

	revert();
}

void GUI_StreamPreferences::retranslate_ui()
{
	ui->retranslateUi(this);
}
