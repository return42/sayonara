/* GUILibraryInfoBox.h

 * Copyright (C) 2011-2017 Lucio Carreras  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 2, 2012 
 *
 */

#ifndef GUILIBRARYINFOBOX_H_
#define GUILIBRARYINFOBOX_H_

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"

namespace Ui { class GUI_LibraryInfoBox; }

class GUI_LibraryInfoBox :
		public SayonaraDialog
{
	Q_OBJECT

public:
	explicit GUI_LibraryInfoBox(int8_t library_id, QWidget* parent=nullptr);
	virtual ~GUI_LibraryInfoBox();

public slots:
	void psl_refresh();

protected:
	void language_changed() override;
	void skin_changed() override;

private:
	Ui::GUI_LibraryInfoBox*	ui=nullptr;
	int8_t _library_id;
};

#endif /* GUILIBRARYINFOBOX_H_ */
