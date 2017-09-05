/* LocalLibraryContainer.h */

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

#ifndef LOCALLIBRARYCONTAINER_H
#define LOCALLIBRARYCONTAINER_H

#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Helper/Pimpl.h"

class LibraryInfo;
class LocalLibraryContainer :
	public LibraryContainerInterface
{
	Q_OBJECT
	PIMPL(LocalLibraryContainer)

public:
	explicit LocalLibraryContainer(const LibraryInfo& library, QObject* parent=nullptr);
	virtual ~LocalLibraryContainer();

	// override from LibraryViewInterface
	QString     name() const override;
	QString     display_name() const override;
	QWidget*    widget() const override;
	QMenu*      menu() override;
	QFrame*		header() const override;
	QPixmap		icon() const override;
	void        init_ui() override;
	int8_t		id();

	void		set_name(const QString& name);
};

#endif // LOCALLIBRARYCONTAINER_H
