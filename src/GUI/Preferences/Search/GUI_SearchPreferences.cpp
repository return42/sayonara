#include "GUI_SearchPreferences.h"
#include "GUI/Preferences/ui_GUI_SearchPreferences.h"

#include "Utils/Language.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/Settings/Settings.h"

GUI_SearchPreferences::GUI_SearchPreferences(const QString& identifier) :
	Preferences::Base (identifier)
{

}

GUI_SearchPreferences::~GUI_SearchPreferences()
{
	if(ui) {
		delete ui; ui=nullptr;
	}
}

QString GUI_SearchPreferences::action_name() const
{
	return Lang::get(Lang::SearchNoun);
}

bool GUI_SearchPreferences::commit()
{
	Library::SearchModeMask mask = 0;

	if(ui->cb_case_insensitive->isChecked()){
		mask |= Library::CaseInsensitve;
	}

	if(ui->cb_no_special_chars->isChecked()){
		mask |= Library::NoSpecialChars;
	}

	if(ui->cb_no_accents->isChecked()){
		mask |= Library::NoDiacriticChars;
	}

	_settings->set(Set::Lib_SearchMode, mask);

	return true;
}

void GUI_SearchPreferences::revert()
{
	Library::SearchModeMask mask = _settings->get(Set::Lib_SearchMode);

	ui->cb_case_insensitive->setChecked(mask & Library::CaseInsensitve);
	ui->cb_no_special_chars->setChecked(mask & Library::NoSpecialChars);
	ui->cb_no_accents->setChecked(mask & Library::NoDiacriticChars);
}

void GUI_SearchPreferences::init_ui()
{
	setup_parent(this, &ui);

	revert();
}

void GUI_SearchPreferences::retranslate_ui()
{
	ui->retranslateUi(this);
}
