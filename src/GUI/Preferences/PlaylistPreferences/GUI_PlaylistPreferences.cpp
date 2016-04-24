#include "GUI_PlaylistPreferences.h"
#include "ui_GUI_PlaylistPreferences.h"

GUI_PlaylistPreferences::GUI_PlaylistPreferences(QWidget *parent) :
	PreferenceWidgetInterface(parent),
	Ui::GUI_PlaylistPreferences()
{

}

GUI_PlaylistPreferences::~GUI_PlaylistPreferences()
{
}



void GUI_PlaylistPreferences::commit()
{
	_settings->set(Set::PL_ShowNumbers, cb_show_numbers->isChecked());
	_settings->set(Set::PL_EntryLook, le_expression->text());
}

void GUI_PlaylistPreferences::revert()
{
	le_expression->setText(_settings->get(Set::PL_EntryLook));
	cb_show_numbers->setChecked(_settings->get(Set::PL_ShowNumbers));
}

void GUI_PlaylistPreferences::init_ui()
{

	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	revert();

	connect(btn_default, &QPushButton::clicked, [=](){
		le_expression->setText("*%title%* - %artist%");
	});
}

QString GUI_PlaylistPreferences::get_action_name() const
{
	return tr("Playlist");
}

void GUI_PlaylistPreferences::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceWidgetInterface::language_changed();
}
