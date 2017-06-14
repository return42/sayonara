/* LibraryPluginLoader.h */

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

#ifndef LIBRARYPLUGINLOADER_H
#define LIBRARYPLUGINLOADER_H

#include "Helper/Settings/SayonaraClass.h"
#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"

#include <QList>
#include <QObject>

class QMenu;
class LibraryInfo;
class LibraryContainerInterface;
/**
 * @brief Library Plugin Manager
 * @ingroup LibraryPlugins
 */
class LibraryPluginHandler :
		public QObject,
		protected SayonaraClass
{
	Q_OBJECT
	PIMPL(LibraryPluginHandler)
	SINGLETON(LibraryPluginHandler)

signals:
	void sig_initialized();
	void sig_current_library_changed(const QString& name);

private:
	/**
	 * @brief Init a library. This is used at startup for the current library
	 * or when the index has changed
	 * @param idx
	 */
	void init_library(LibraryContainerInterface* container);


public:
	/**
	 * @brief Search for plugins and add some predefined plugins
	 * @param containers Some predefined plugins
	 */
	void init(const QList<LibraryContainerInterface*>& containers);

	/**
	 * @brief Set the parent widget for library plugins
	 * @param parent Parent widget
	 */
	void set_library_parent(QWidget* parent);




	/**
	 * @brief Get a list for all found plugins. The ui is not necessarily initialized
	 * @return list for all found library plugins
	 */
	QList<LibraryContainerInterface*> get_libraries() const;

	void add_local_library(const LibraryInfo& library);
	void rename_local_library(qint8 library_id, const QString& new_name);
	void remove_local_library(qint8 library_id);
	void move_local_library(int old_row, int new_row);

	LibraryContainerInterface* current_library() const;
	QMenu* current_library_menu() const;

	static bool is_local_library(const LibraryContainerInterface* container);

private slots:
	void current_library_changed(int library_idx);

public slots:
	void set_current_library(const QString& name);
	void set_current_library(LibraryContainerInterface* container);
};

#endif // LIBRARYPLUGINLOADER_H
