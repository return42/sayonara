/* LibraryPluginCombobox.cpp */

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



#include "LibraryPluginCombobox.h"
#include "LibraryPluginHandler.h"
#include "LibraryContainer/LibraryContainer.h"
#include "Utils/Settings/Settings.h"

#include <QList>
#include <QAction>
#include <QSize>

struct LibraryPluginCombobox::Private
{
	LibraryPluginHandler* lph=nullptr;
	QList<QAction*> actions;

	Private()
	{
		lph = LibraryPluginHandler::instance();
	}
};


LibraryPluginCombobox::LibraryPluginCombobox(const QString& text, QWidget* parent) :
	ComboBox(parent)
{
	m = Pimpl::make<Private>();

	this->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->setFrame(false);
    this->setIconSize(QSize(16, 16));
    this->setFocusPolicy(Qt::ClickFocus);

	connect(m->lph, &LibraryPluginHandler::sig_initialized, this, &LibraryPluginCombobox::setup_actions);
	connect(m->lph, &LibraryPluginHandler::sig_libraries_changed, this, &LibraryPluginCombobox::setup_actions);
	connect(m->lph, &LibraryPluginHandler::sig_current_library_changed, this, &LibraryPluginCombobox::current_library_changed);

    Set::listen(Set::Player_Language, this, &LibraryPluginCombobox::setup_actions, false);

    setup_actions();
	setCurrentText(text);
}

LibraryPluginCombobox::~LibraryPluginCombobox() {}

void LibraryPluginCombobox::setup_actions()
{
	this->clear();

	QList<LibraryContainerInterface*> libraries = m->lph->get_libraries();

	for(const LibraryContainerInterface* container : libraries)
	{
        QPixmap pm = container->icon().scaled(
                    this->iconSize(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
        );

        this->addItem(QIcon(pm), container->display_name(), container->name());
	}

	current_library_changed(m->lph->current_library()->name());
}

void LibraryPluginCombobox::action_triggered(bool b)
{
	if(!b){
		return;
	}

	QAction* action = static_cast<QAction*>(sender());
	QString name = action->data().toString();

	LibraryPluginHandler::instance()->set_current_library(name);
	for(QAction* library_action : m->actions)
	{
		if(library_action == action){
			continue;
		}

		library_action->setChecked(false);
	}
}

void LibraryPluginCombobox::current_library_changed(const QString& name)
{
	for(int i=0; i<this->count(); i++) {
		if(this->itemData(i).toString().compare(name) == 0){
			this->setCurrentIndex(i);
			break;
		}
	}
}
