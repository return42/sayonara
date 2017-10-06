/* DirectoryWidgetContainer.cpp */

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

#include "DirectoryWidgetContainer.h"
#include "GUI/Helper/GUI_Helper.h"

#include "GUI_DirectoryWidget.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "Helper/Language.h"

#include <QIcon>
#include <QPixmap>

DirectoryLibraryContainer::DirectoryLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent) {}

DirectoryLibraryContainer::~DirectoryLibraryContainer() {}

QString DirectoryLibraryContainer::name() const
{
	return "directories";
}

QString DirectoryLibraryContainer::display_name() const
{
	return Lang::get(Lang::Directories);
}

QWidget* DirectoryLibraryContainer::widget() const
{
	return static_cast<QWidget*>(ui);
}

void DirectoryLibraryContainer::init_ui()
{
	ui = new GUI_DirectoryWidget(nullptr);
}


QFrame* DirectoryLibraryContainer::header() const
{
	return ui->header_frame();
}

QPixmap DirectoryLibraryContainer::icon() const
{
    QIcon icon = IconLoader::instance()->icon("folder", "dir_view");
	QList<QSize> sizes = icon.availableSizes();

	auto it = std::max_element(sizes.begin(), sizes.end(), [](const QSize& sz1, const QSize& sz2){
		return (sz1.height() < sz2.height());
	});

	return icon.pixmap(*it);
}
