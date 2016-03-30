/* GUI_LanguageChooser.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "GUI_LanguageChooser.h"
#include "GUI/Helper/ComboBoxDelegate/ComboBoxDelegate.h"
#include "Helper/Helper.h"

#include <QFile>
#include <QDir>

GUI_LanguageChooser::GUI_LanguageChooser(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_LanguageChooser()
{

}

GUI_LanguageChooser::~GUI_LanguageChooser() {

}

void GUI_LanguageChooser::language_changed(){

	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	PreferenceDialogInterface::language_changed();
}


void GUI_LanguageChooser::commit() {

	int cur_idx = combo_lang->currentIndex();
	QString cur_language = combo_lang->itemData(cur_idx).toString();

	_settings->set(Set::Player_Language, cur_language);
}

void GUI_LanguageChooser::revert() {

}


void GUI_LanguageChooser::renew_combo() {

	if(!is_ui_initialized()){
		return;
	}

	QString lang_setting = _settings->get(Set::Player_Language);
	sp_log(Log::Info) << "Language setting = " << lang_setting;
    QDir dir(Helper::get_share_path() + "translations/");

	QStringList filters;
		filters << "*.qm";
    QStringList files = dir.entryList(filters);

	combo_lang->clear();
	//combo_lang->addItem(tr("Default"), "default");

    int i=0;
	for(const QString file : files) {

		QString filename, dirname;
		Helper::File::split_filename(file, dirname, filename);

		filename = filename.left(filename.size() - 3);


		QString two = filename;
		two.replace("sayonara_lang_", "");

        QString title = _map.value(two);
		QString flag = Helper::get_share_path() + "translations/icons/" + two + ".png";

		if(title.size() > 0){
			combo_lang->addItem(QIcon(flag), title, filename);
		}

		else{
			combo_lang->addItem(filename, filename);
		}

		if(filename.compare(lang_setting, Qt::CaseInsensitive) == 0){
			combo_lang->setCurrentIndex(i);
		}

		i++;
	}
}

void GUI_LanguageChooser::init_ui()
{
	setup_parent(this);

	_map["br"] = QString::fromUtf8("Português (Brasil)");
	_map["cs"] = QString::fromUtf8("Český");
	_map["de"] = "Deutsch";
	_map["en"] = "English";
	_map["es"] = QString::fromUtf8("Español");
	_map["mx"] = QString::fromUtf8("Español (México)");
	_map["fr"] = "Francais";
	_map["hu"] = "Magyar";
	_map["it"] = "Italiano";
	_map["pl"] = QString::fromUtf8("Polski");
	_map["pt"] = QString::fromUtf8("Português");
	_map["ro"] = QString::fromUtf8("Limba română");
	_map["ru"] = QString::fromUtf8("Русский");
	_map["ua"] = QString::fromUtf8("Українська");
	_map["zh_cn"] = QString::fromUtf8("中文");

	combo_lang->setItemDelegate(new ComboBoxDelegate(combo_lang));
}

void GUI_LanguageChooser::showEvent(QShowEvent* e) {

	PreferenceDialogInterface::showEvent(e);

	renew_combo();
}


QString GUI_LanguageChooser::get_action_name() const
{
	return tr("Language") + QString(" (Language)");
}

QLabel*GUI_LanguageChooser::get_title_label()
{
	return lab_title;
}
