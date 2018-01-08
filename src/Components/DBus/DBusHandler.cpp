/* DBusHandler.cpp */

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

#include "DBusHandler.h"
#include "Components/DBus/DBusMPRIS.h"
#include "Components/DBus/DBusMediaKeysInterfaceMate.h"
#include "Components/DBus/DBusMediaKeysInterfaceGnome.h"
#include "Components/DBus/DBusNotifications.h"

#include "GUI/Utils/Widgets/Widget.h"

#include <QDBusConnectionInterface>

DBusHandler::DBusHandler(QMainWindow* player, QObject* parent) :
	QObject(parent)
{
	_dbus_mpris	= new DBusMPRIS::MediaPlayer2(player, this);
	_dbus_mate = new DBusMediaKeysInterfaceMate(this);
	_dbus_gnome = new DBusMediaKeysInterfaceGnome(this);
	_dbus_notifications = new DBusNotifications(this);

	QDBusConnectionInterface* dbus_interface = QDBusConnection::sessionBus().interface();
	if(dbus_interface)
	{
		connect(dbus_interface, &QDBusConnectionInterface::serviceRegistered,
				this, &DBusMediaKeysInterface::service_registered);
		connect(dbus_interface, &QDBusConnectionInterface::serviceUnregistered,
				this, &DBusMediaKeysInterface::service_unregistered);
	}
}

DBusHandler::~DBusHandler() {}


void DBusHandler::service_registered(const QString& service_name)
{
	sp_log(Log::Info, this) << "Service " << service_name << " registered";
}

void DBusHandler::service_unregistered(const QString& service_name)
{
	sp_log(Log::Warning, this) << "Service " << service_name << " unregistered";
}
