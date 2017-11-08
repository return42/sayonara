/* GUI_PreferenceDialog.cpp */

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

#include "GUI_PreferenceDialog.h"
#include "GUI/Preferences/ui_GUI_PreferenceDialog.h"

#include "GUI/Utils/Delegates/StyledItemDelegate.h"
#include "Interfaces/PreferenceDialog/PreferenceWidget.h"
#include "Interfaces/PreferenceDialog/PreferenceAction.h"
#include "Utils/globals.h"

#include <QLayout>

using Preferences::Base;
using Preferences::Action;

struct GUI_PreferenceDialog::Private
{
	QList<Base*>	pref_widgets;
	Action*			action=nullptr;
};

GUI_PreferenceDialog::GUI_PreferenceDialog(QWidget *parent) :
	PreferenceDialog(parent)
{
	m = Pimpl::make<Private>();
}

GUI_PreferenceDialog::~GUI_PreferenceDialog()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}

void GUI_PreferenceDialog::register_preference_dialog(Base* pref_widget)
{
	m->pref_widgets << pref_widget;
}

void GUI_PreferenceDialog::show_preference_dialog(const QString& identifier)
{
	int i=0;
	for(Preferences::Base* pwi : m->pref_widgets)
	{
		if(identifier.compare(pwi->identifier()) == 0)
		{
			row_changed(i);
			return;
		}

		i++;
	}

	sp_log(Log::Warning, this) << "Cannot find preference widget " << identifier;
}


void GUI_PreferenceDialog::language_changed()
{
	ui->retranslateUi(this);

	// TODO: Init should not be so deep in language changed
	bool is_empty = (ui->list_preferences->count() == 0);

	int i=0;
	for(Base* dialog : m->pref_widgets)
	{
		QListWidgetItem* item;
		if(is_empty){
			item = new QListWidgetItem(dialog->action_name());
			ui->list_preferences->addItem(item);
		}
		else{
			item = ui->list_preferences->item(i);
			item->setText(dialog->action_name());
		}

		i++;
	}
}


QString GUI_PreferenceDialog::action_name() const
{
	return tr("Preferences");
}

QAction* GUI_PreferenceDialog::action()
{
	// action has to be initialized here, because pure
	// virtual get_action_name should not be called from ctor
	QString name = action_name();
	if(!m->action){
		m->action = new Action(name, this);
	}

	m->action->setText(name + "...");
	return m->action;
}


void GUI_PreferenceDialog::commit_and_close()
{
	commit();
	close();
}


void GUI_PreferenceDialog::commit()
{
	for(Base* iface : m->pref_widgets){
		if(iface->is_ui_initialized()){
			iface->commit();
		}
	}
}


void GUI_PreferenceDialog::revert()
{
	for(Base* iface : m->pref_widgets){
		if(iface->is_ui_initialized()){
			iface->revert();
		}
	}

	close();
}


void GUI_PreferenceDialog::row_changed(int row)
{
	if(!between(row, m->pref_widgets)){
		return;
	}

	hide_all();

	Base* widget = m->pref_widgets[row];

	QLayout* layout = ui->widget_preferences->layout();
	layout->setContentsMargins(0,0,0,0);

	if(layout){
		layout->addWidget(widget);
		layout->setAlignment(Qt::AlignTop);
	}

	ui->lab_pref_title->setText(widget->action_name());

	widget->show();
}


void GUI_PreferenceDialog::hide_all()
{
	for(Base* iface : m->pref_widgets){
		iface->setParent(nullptr);
		iface->hide();
	}
}

void GUI_PreferenceDialog::showEvent(QShowEvent* e)
{
	init_ui();
	Dialog::showEvent(e);
}


void GUI_PreferenceDialog::init_ui()
{
	if(ui){
		return;
	}

	ui = new Ui::GUI_PreferenceDialog();
	ui->setupUi(this);

	for(Base* widget : m->pref_widgets)
	{
		ui->list_preferences->addItem(widget->action_name());
	}

	ui->list_preferences->setMouseTracking(false);
	ui->list_preferences->setItemDelegate(
		new Gui::StyledItemDelegate(ui->list_preferences)
	);

	connect(ui->list_preferences, &QListWidget::currentRowChanged, this, &GUI_PreferenceDialog::row_changed);
	connect(ui->btn_apply, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit);
	connect(ui->btn_ok, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit_and_close);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_PreferenceDialog::revert);
}
