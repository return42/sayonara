/* GUI_LanguageChooser.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
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
#include "GUI/Preferences/ui_GUI_LanguageChooser.h"

#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include <QFile>
#include <QDir>
#include <QRegExp>

struct GUI_LanguageChooser::Private
{
	QMap<QString, QString>  map;
};

GUI_LanguageChooser::GUI_LanguageChooser(const QString& identifier) :
	Preferences::Base(identifier)
{
	m = Pimpl::make<Private>();
}

GUI_LanguageChooser::~GUI_LanguageChooser()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}


void GUI_LanguageChooser::retranslate_ui()
{
	ui->retranslateUi(this);
	renew_combo();
}


bool GUI_LanguageChooser::commit()
{
	int cur_idx = ui->combo_lang->currentIndex();
	QString cur_language = ui->combo_lang->itemData(cur_idx).toString();

	_settings->set(Set::Player_Language, cur_language);

	return true;
}

void GUI_LanguageChooser::revert() {}

// typically a qm file looks like sayonara_lang_lc.qm
void GUI_LanguageChooser::renew_combo()
{
	if(!is_ui_initialized()){
		return;
	}

	QString lang_setting = _settings->get(Set::Player_Language);

	sp_log(Log::Info, this) << "Language setting = " << lang_setting;
	QDir dir(Util::share_path("translations"));

	QStringList filters;
	filters << "*.qm";
	QStringList files = dir.entryList(filters);

	ui->combo_lang->clear();

	int i=0;
	for(const QString& file : files)
	{
		QRegExp re(".*lang_(.*)\\.qm");
		re.setMinimal(true);

		QString country_code;
		if(re.indexIn(file) >= 0){
			country_code = re.cap(1).toLower();
		}

		else{
			continue;
		}

		if(country_code.compare("mx") == 0){
			continue;
		}

		QString icon_path = Util::share_path(
					"translations/icons/" + country_code + ".png"
		);

		QString language_name = m->map.value(country_code);

		if(language_name.size() > 0){
			ui->combo_lang->addItem(QIcon(icon_path), language_name, file);
		}

		else{
			ui->combo_lang->addItem(file, file);
		}

		if(file.contains(lang_setting, Qt::CaseInsensitive)){
			ui->combo_lang->setCurrentIndex(i);
		}

		i++;
	}
}


void GUI_LanguageChooser::init_ui()
{
	setup_parent(this, &ui);

	m->map["br"] = QString::fromUtf8("Português (Brasil)");
	m->map["cs"] = QString::fromUtf8("Český");
	m->map["de"] = "Deutsch";
	m->map["en"] = "English";
	m->map["es"] = QString::fromUtf8("Español");
	m->map["fr"] = "Francais";
	m->map["hu"] = "Magyar";
	m->map["it"] = "Italiano";
	m->map["ja"] = QString::fromUtf8("日本語");
	m->map["nl"] = "Nederlands";
	m->map["pl"] = QString::fromUtf8("Polski");
	m->map["pt"] = QString::fromUtf8("Português");
	m->map["ro"] = QString::fromUtf8("Limba română");
	m->map["ru"] = QString::fromUtf8("Русский");
	m->map["tr"] = QString::fromUtf8("Türkçe");
	m->map["ua"] = QString::fromUtf8("Українська");
	m->map["zh_cn"] = QString::fromUtf8("中文");
}

void GUI_LanguageChooser::showEvent(QShowEvent* e)
{
	Base::showEvent(e);

	renew_combo();
}

QString GUI_LanguageChooser::action_name() const
{
	return tr("Language") + QString(" (Language)");
}

