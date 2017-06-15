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

#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include "GUI/Helper/Delegates/StyledItemDelegate.h"

#include <QListWidgetItem>
#include <QList>

GUI_Covers::GUI_Covers(QWidget* parent) :
	PreferenceWidgetInterface (parent) {}


GUI_Covers::~GUI_Covers()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}

void GUI_Covers::commit()
{
	Settings* settings = Settings::getInstance();
	QStringList active_items;
	for(int i=0; i<ui->lv_active->count(); i++){
		QListWidgetItem* item = ui->lv_active->item(i);
		active_items << item->text();
	}

	settings->set(Set::Cover_Server, active_items);
}

void GUI_Covers::revert()
{
	Settings* settings = Settings::getInstance();
	QStringList active = settings->get(Set::Cover_Server);

	ui->lv_active->clear();
	ui->lv_inactive->clear();

	CoverFetchManager* cfm = CoverFetchManager::getInstance();

	QList<CoverFetcherInterface*> cfis = cfm->get_available_coverfetchers();
	for(const CoverFetcherInterface* cfi : cfis) {
		if(cfi->get_keyword().isEmpty()){
			continue;
		}

		if(active.contains(cfi->get_keyword())){
			ui->lv_active->addItem(cfi->get_keyword());
		}

		else{
			ui->lv_inactive->addItem(cfi->get_keyword());
		}
	}
}

QString GUI_Covers::get_action_name() const
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

	ui->lv_active->setItemDelegate(new StyledItemDelegate(ui->lv_active));
	ui->lv_inactive->setItemDelegate(new StyledItemDelegate(ui->lv_inactive));

	revert();
}

void GUI_Covers::retranslate_ui()
{
	ui->retranslateUi(this);
	ui->lab_active->setText(Lang::get(Lang::Active));
	ui->lab_inactive->setText(Lang::get(Lang::Inactive));
}
