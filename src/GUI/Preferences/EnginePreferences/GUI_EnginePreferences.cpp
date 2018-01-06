#include "GUI_EnginePreferences.h"
#include "GUI/Preferences/ui_GUI_EnginePreferences.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"
#include <asoundlib.h>

GUI_EnginePreferences::GUI_EnginePreferences(const QString& identifier) :
	Preferences::Base(identifier)
{}


GUI_EnginePreferences::~GUI_EnginePreferences()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}


QString GUI_EnginePreferences::action_name() const
{
	return tr("Audio");
}

bool GUI_EnginePreferences::commit()
{
	if(ui->rb_pulse->isChecked()){
		_settings->set(Set::Engine_Sink, QString("pulse"));
	}

	else if(ui->rb_alsa->isChecked()){
		_settings->set(Set::Engine_Sink, QString("alsa"));
	}

	else{
		_settings->set(Set::Engine_Sink, QString("auto"));
	}

	return true;
}

void GUI_EnginePreferences::revert()
{
	QString engine_name = _settings->get(Set::Engine_Sink);
	if(engine_name == "pulse"){
		ui->rb_pulse->setChecked(true);
	}

	else if(engine_name == "alsa"){
		ui->rb_alsa->setChecked(true);
	}

	else{
		ui->rb_auto->setChecked(true);
	}
}

void GUI_EnginePreferences::init_ui()
{
	if(ui){
		return;
	}

	setup_parent(this, &ui);

	revert();
}

void GUI_EnginePreferences::retranslate_ui()
{
	ui->retranslateUi(this);
}

