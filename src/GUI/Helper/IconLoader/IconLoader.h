/* IconLoader.h */

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

#ifndef ICON_LOADER_H_
#define ICON_LOADER_H_

#include <QObject>
#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"

class Settings;
class QString;
class QStringList;
class QIcon;
class QPixmap;

/**
 * @brief The IconLoader class
 * @ingroup GUIHelper
 */
class IconLoader :
		public QObject
{
	Q_OBJECT
	SINGLETON(IconLoader)
	PIMPL(IconLoader)

private slots:
	void icon_theme_changed();

public:
	void add_icon_names(const QStringList& icon_names);
	QIcon get_icon(const QString& name, const QString& dark_name);
	QIcon get_icon(const QStringList& names, const QString& dark_name);
	bool has_std_icon(const QString& name) const;
};

#endif
