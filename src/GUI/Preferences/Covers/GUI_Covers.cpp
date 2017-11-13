/* GUI_Covers.cpp */

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

#include "GUI/Preferences/ui_GUI_Covers.h"

#include "GUI_Covers.h"

#include "Components/Covers/CoverFetchManager.h"
#include "Components/Covers/CoverFetcherInterface.h"

#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"

#include "GUI/Utils/Delegates/StyledItemDelegate.h"

#include <QListWidgetItem>
#include <QList>

using namespace Cover;

GUI_Covers::GUI_Covers(const QString& identifier) :
	Base (identifier) {}


GUI_Covers::~GUI_Covers()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}

bool GUI_Covers::commit()
{
	Settings* settings = Settings::instance();
	QStringList active_items;

	for(int i=0; i<ui->lv_active->count(); i++)
	{
		QListWidgetItem* item = ui->lv_active->item(i);
		active_items << item->text();
	}

	settings->set(Set::Cover_Server, active_items);
	settings->set(Set::Cover_LoadFromFile, ui->cb_load_covers_from_file->isChecked());
}

void GUI_Covers::revert()
{
	Settings* settings = Settings::instance();
	QStringList active = settings->get(Set::Cover_Server);

	ui->lv_active->clear();
	ui->lv_inactive->clear();

	Fetcher::Manager* cfm = Fetcher::Manager::instance();

	QList<Fetcher::Base*> cfis = cfm->available_coverfetchers();
	for(const Fetcher::Base* cfi : cfis)
	{
		if(cfi->keyword().isEmpty()){
			continue;
		}

		if(active.contains(cfi->keyword())){
			ui->lv_active->addItem(cfi->keyword());
		}

		else{
			ui->lv_inactive->addItem(cfi->keyword());
		}
	}

	ui->cb_load_covers_from_file->setChecked(settings->get(Set::Cover_LoadFromFile));
}

QString GUI_Covers::action_name() const
{
	return Lang::get(Lang::Covers);
}

void GUI_Covers::init_ui()
{
	if(ui){
		return;
	}

	setup_parent(this, &ui);
	ui->lv_active->clear();
	ui->lv_inactive->clear();

	ui->lv_active->setItemDelegate(new Gui::StyledItemDelegate(ui->lv_active));
	ui->lv_inactive->setItemDelegate(new Gui::StyledItemDelegate(ui->lv_inactive));

	revert();
}

void GUI_Covers::retranslate_ui()
{
	ui->retranslateUi(this);
	ui->lab_active->setText(Lang::get(Lang::Active));
	ui->lab_inactive->setText(Lang::get(Lang::Inactive));
}
