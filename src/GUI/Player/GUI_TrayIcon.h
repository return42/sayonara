/* GUI_TrayIcon.h */

/* Copyright (C) 2011-2017 Lucio Carreras  gleugner
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

#ifndef GUI_TRAYICON_H
#define GUI_TRAYICON_H



#include "Interfaces/Notification/NotificationHandler.h"
#include "Components/PlayManager/PlayState.h"

#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"

#include <QSystemTrayIcon>

class QTimer;
class QAction;
class PlayManager;
class MetaData;
/**
  * Small class to be used as tray icon
  */
class GUI_TrayIcon :
		public QSystemTrayIcon,
        public NotificationInterface,
        public SayonaraClass
{
	Q_OBJECT
    PIMPL(GUI_TrayIcon)

public:

	explicit GUI_TrayIcon(QObject *parent=nullptr);
	virtual ~GUI_TrayIcon();

	bool event ( QEvent * e ) override;
	void set_enable_fwd(bool);

	void notify(const MetaData& md) override;
	void notify(const QString &title, const QString &message, const QString &image_path) override;


signals:

	/**
	  * this event is fired, if we have a mouse wheel event
	  * @param delta bigger then 0 when mouse wheel has moved forward smaller when moved backwards
	  */
	void sig_wheel_changed(int delta);
	void sig_hide_clicked();
	void sig_close_clicked();
	void sig_show_clicked();


private slots:
	void play_clicked();
	void stop_clicked();
	void fwd_clicked();
	void bwd_clicked();
	void show_clicked();
	void close_clicked();
	void mute_clicked();
	void cur_song_clicked();

	void playstate_changed(PlayState state);

	void mute_changed(bool muted);
	void _sl_show_tray_icon();
	
	void language_changed();
	void skin_changed();

private:
	void init_context_menu();
};

#endif
