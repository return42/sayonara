/* PlayerPlugin.cpp */

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

#include "PlayerPlugin.h"
#include "GUI/Helper/Shortcuts/Shortcut.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"
#include "Helper/Settings/Settings.h"

#include <QAction>
#include <QLayout>
#include <QCloseEvent>

struct PlayerPluginInterface::Private
{
	bool		is_initialized;
	QAction*	pp_action=nullptr;

	Private()
	{
		is_initialized = false;
		pp_action = new QAction(nullptr);
		pp_action->setCheckable(true);
	}

	~Private()
	{
		delete pp_action; pp_action = nullptr;
	}
};

PlayerPluginInterface::PlayerPluginInterface(QWidget *parent) :
	SayonaraWidget(parent),
	ShortcutWidget()
{
	_m = Pimpl::make<Private>();

	connect(_m->pp_action, &QAction::triggered, this, &PlayerPluginInterface::action_triggered);

	hide();
}


PlayerPluginInterface::~PlayerPluginInterface() {}


bool PlayerPluginInterface::is_title_shown() const
{
	return true;
}


QString PlayerPluginInterface::get_shortcut_text(const QString& shortcut_identifier) const
{
	if(shortcut_identifier == "close_plugin"){
		return tr("Close plugin");
	}

	return "";
}


QAction* PlayerPluginInterface::get_action() const 
{
	_m->pp_action->setText( this->get_display_name() );
	return _m->pp_action;
}


QSize PlayerPluginInterface::get_size() const
{
	return this->minimumSize();
}


void PlayerPluginInterface::finalize_initialization()
{
	QLayout* widget_layout = layout();
	if(widget_layout){
		widget_layout->setContentsMargins(3, 3, 3, 3);
	}

	ShortcutHandler* sch = ShortcutHandler::getInstance();
	Shortcut sc = sch->get_shortcut("close_plugin");
	if(!sc.is_valid()){
		sc = sch->add(Shortcut(this, "close_plugin", tr("Close plugin"), "Ctrl+Esc"));
	}

	if(parentWidget()){
		sc.create_qt_shortcut(this, parentWidget(), SLOT(close()));
	}

	REGISTER_LISTENER(Set::Player_Style, skin_changed);

	this->set_ui_initialized();
}


bool PlayerPluginInterface::is_ui_initialized() const
{
	return _m->is_initialized;
}


void PlayerPluginInterface::set_ui_initialized()
{
	_m->is_initialized = true;
}


void PlayerPluginInterface::showEvent(QShowEvent* e)
{
	if(!is_ui_initialized()){
		init_ui();
	}

	QWidget::showEvent(e);
}


void PlayerPluginInterface::closeEvent(QCloseEvent* e)
{
	SayonaraWidget::closeEvent(e);

	_m->pp_action->setChecked(false);

	emit sig_closed();
}


void PlayerPluginInterface::action_triggered(bool b) 
{
	_m->pp_action->setChecked(b);

	emit sig_action_triggered(this, b);

	skin_changed();
}


