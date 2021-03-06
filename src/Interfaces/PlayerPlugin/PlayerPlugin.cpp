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
#include "GUI/Utils/Shortcuts/Shortcut.h"
#include "GUI/Utils/Shortcuts/ShortcutHandler.h"
#include "Utils/Settings/Settings.h"

#include <QAction>
#include <QLayout>
#include <QCloseEvent>

using PlayerPlugin::Base;

struct Base::Private
{
	QAction*	pp_action=nullptr;
	bool		is_initialized;

	Private() :
		pp_action(new QAction(nullptr)),
		is_initialized(false)
	{
		pp_action->setCheckable(true);
	}

	~Private()
	{
		delete pp_action; pp_action = nullptr;
	}
};

Base::Base(QWidget *parent) :
	Widget(parent),
	ShortcutWidget()
{
	m = Pimpl::make<Private>();

	connect(m->pp_action, &QAction::triggered, this, &Base::action_triggered);

	hide();
}


Base::~Base() {}

void Base::skin_changed() {}

bool Base::is_title_shown() const
{
	return true;
}

bool Base::has_loading_bar() const
{
	return false;
}


QString Base::get_shortcut_text(const QString& shortcut_identifier) const
{
	if(shortcut_identifier == "close_plugin"){
		return tr("Close plugin");
	}

	return "";
}


QAction* Base::get_action() const
{
	m->pp_action->setText( this->get_display_name() );
	return m->pp_action;
}


QSize Base::get_size() const
{
	return this->minimumSize();
}

void Base::finalize_initialization()
{
	QLayout* widget_layout = layout();
	if(widget_layout){
		int bottom = 3;
		if(has_loading_bar()){
			bottom = 10;
		}

		widget_layout->setContentsMargins(3, 3, 3, bottom);
	}

	ShortcutHandler* sch = ShortcutHandler::instance();
	Shortcut sc = sch->get_shortcut("close_plugin");
	if(!sc.is_valid()){
		sc = sch->add(Shortcut(this, "close_plugin", tr("Close plugin"), "Ctrl+Esc"));
	}

	if(parentWidget()){
		sc.create_qt_shortcut(this, parentWidget(), SLOT(close()));
	}

	Set::listen(Set::Player_Style, this, &Base::skin_changed);

	set_ui_initialized();
	retranslate_ui();
}

void Base::assign_ui_vars() {}

void Base::language_changed()
{
	if(is_ui_initialized()){
		retranslate_ui();
	}
}


bool Base::is_ui_initialized() const
{
	return m->is_initialized;
}


void Base::set_ui_initialized()
{
	m->is_initialized = true;
}


void Base::showEvent(QShowEvent* e)
{
	if(!is_ui_initialized()){
		init_ui();
	}

	Widget::showEvent(e);

	m->pp_action->setChecked(true);

	emit sig_opened();
}


void Base::closeEvent(QCloseEvent* e)
{
	Widget::closeEvent(e);

	m->pp_action->setChecked(false);

	emit sig_closed();
}


void Base::action_triggered(bool b)
{
	m->pp_action->setChecked(b);

	emit sig_action_triggered(b);

	skin_changed();
}


