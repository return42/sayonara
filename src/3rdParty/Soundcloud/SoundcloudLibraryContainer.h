/* SoundcloudLibraryContainer.h */

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


/* SoundcloudLibraryContainer.h */

#ifndef SOUNDCLOUD_LIBRARY_CONTAINER
#define SOUNDCLOUD_LIBRARY_CONTAINER

#include <QtGlobal>
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"

namespace SC
{
	class GUI_Library;

	class LibraryContainer :
		public ::Library::Container
	{
		Q_OBJECT

		Q_PLUGIN_METADATA(IID "com.sayonara-player.soundcloud_library")
		Q_INTERFACES(LibraryContainerInterface)

	private:
		SC::GUI_Library*	ui=nullptr;

	public:

		explicit LibraryContainer(QObject* parent=nullptr);
		~LibraryContainer();

		// override from LibraryViewInterface
		QString			name() const override;
		QString			display_name() const override;
		QWidget*		widget() const override;
		QMenu*			menu() override;
		QFrame*			header() const override;
		QPixmap			icon() const override;
		void			init_ui() override;
	};
}

#endif
