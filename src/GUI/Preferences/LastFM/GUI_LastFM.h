/* GUI_LastFM.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * GUI_LastFM.h
 *
 *  Created on: Apr 21, 2011
 *      Author: Lucio Carreras
 */

#ifndef GUI_LASTFM_H_
#define GUI_LASTFM_H_

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

namespace Ui
{
	class GUI_LastFM;
}

class LastFM;

class GUI_LastFM :
		public PreferenceWidgetInterface
{
	Q_OBJECT

public:
	explicit GUI_LastFM(QWidget* parent=nullptr);
	virtual ~GUI_LastFM();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;

protected:
	void init_ui() override;
	void retranslate_ui() override;

private:
	Ui::GUI_LastFM*		ui=nullptr;
	LastFM*				_lfm=nullptr;


private slots:

	void btn_login_clicked();
	void active_changed(bool active);
	void logged_in(bool success);
};

#endif /* GUI_LASTFM_H_ */
