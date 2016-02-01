/* GUI_Notifications.h */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#ifndef GUI_NOTIFICATIONS_H
#define GUI_NOTIFICATIONS_H

#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"
#include "Interfaces/Notification/NotificationHandler.h"
#include "GUI/Notifications/ui_GUI_Notifications.h"


class GUI_Notifications :
		public PreferenceDialogInterface,
		private Ui_GUI_Notification
{
    Q_OBJECT

	friend class PreferenceDialogInterface;


public:
    explicit GUI_Notifications(QWidget *parent=nullptr);
    virtual ~GUI_Notifications();

    
private slots:
	void notifications_changed();
    void ok_clicked();



protected:
	void language_changed() override;
	QString get_action_name() const override;


private:
	NotificationHandler* _notification_handler=nullptr;

private:
	void init_ui() override;

};



#endif // GUI_NOTIFICATIONS_H
