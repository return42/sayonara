/* GUI_PlayerPreferences.h */

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

#ifndef GUI_PLAYERPREFERENCES_H
#define GUI_PLAYERPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

namespace Ui
{
	class GUI_PlayerPreferences;
}

class GUI_PlayerPreferences :
		public PreferenceWidgetInterface
{
	Q_OBJECT

public:
	explicit GUI_PlayerPreferences(QWidget *parent = 0);
	virtual ~GUI_PlayerPreferences();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;


protected:
	void init_ui() override;
	void retranslate_ui() override;


private:
	 Ui::GUI_PlayerPreferences*	ui=nullptr;

private slots:
	void show_tray_icon_toggled(bool b);
};

#endif // GUI_PLAYERPREFERENCES_H
