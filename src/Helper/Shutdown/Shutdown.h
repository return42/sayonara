/* Shutdown.h */

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

#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#ifdef WITH_DBUS
	#define WITH_SHUTDOWN
#endif

#ifdef WITH_SHUTDOWN


#include "Helper/Singleton.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QDBusInterface>
#include <QDBusConnection>
#include <QProcess>
#include <QTimer>

class PlayManager;
class NotificationHandler;

/**
 * @brief The Shutdown class
 * @ingroup Helper
 */
class Shutdown : public QObject, private SayonaraClass 
{
	Q_OBJECT
	SINGLETON_QOBJECT(Shutdown)

signals:

	void sig_time_to_go(uint64_t);
	void sig_started(uint64_t);


private:
	QTimer*			_timer=nullptr;
	QTimer*			_timer_countdown=nullptr;
	PlayManager*	_play_manager=nullptr;

	uint64_t			_msecs2go;
	bool			_is_running;


private slots:
	void timeout();
	void countdown_timeout();
	void playlist_finished();

public:

	bool is_running() const;
	void stop();
	void shutdown(uint64_t ms=0);
	void shutdown_after_end();
};

#endif // WITH_SHUTDOWN

#endif // SHUTDOWN_H
