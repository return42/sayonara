/* GUI_Covers.h */

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



#ifndef GUI_COVERS_H
#define GUI_COVERS_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

namespace Ui { class GUI_Covers; }

class GUI_Covers :
		public PreferenceWidgetInterface
{

public:
	GUI_Covers(QWidget* parent=nullptr);
	virtual ~GUI_Covers();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;

protected:
	void init_ui() override;
	void retranslate_ui() override;

private:
	Ui::GUI_Covers* ui=nullptr;
};


#endif // GUI_COVERS_H
