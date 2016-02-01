/* SoundcloudLibraryContainer.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "SoundcloudLibraryContainer.h"

SoundcloudLibraryContainer::SoundcloudLibraryContainer(QObject* parent) :
	LibraryContainer(parent)
{
	Q_INIT_RESOURCE(SoundcloudIcons);

	SoundcloudLibrary* sc_library = new SoundcloudLibrary(this);

	_library_ui = (GUI_AbstractLibrary*) (new GUI_SoundCloudLibrary(sc_library));

	_icon = QIcon(":/sc_icons/ui/icon.png") ;
}

SoundcloudLibraryContainer::~SoundcloudLibraryContainer()
{

}


QString SoundcloudLibraryContainer::get_name() const
{
	return tr("Soundcloud");
}

QString SoundcloudLibraryContainer::get_display_name() const
{
	return tr("Soundcloud");
}

