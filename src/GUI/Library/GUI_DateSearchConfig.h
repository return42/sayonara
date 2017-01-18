/* GUI_DateSearchConfig.h */

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

#ifndef GUI_DATESEARCHCONFIG_H
#define GUI_DATESEARCHCONFIG_H

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Helper/Pimpl.h"

namespace Library { class DateFilter; }
namespace Ui{ class GUI_DateSearchConfig; }

class GUI_DateSearchConfig :
		public SayonaraDialog
{
public:

	enum class Result
	{
		Cancelled,
		Replace,
		New
	};

	explicit GUI_DateSearchConfig(QWidget* parent=nullptr);
	~GUI_DateSearchConfig();

	void set_filter(const Library::DateFilter& filter);
	Library::DateFilter get_edited_filter() const;
	Result get_result() const;


protected slots:
	void language_changed() override;

	void ok_clicked();
	void save_as_clicked();
	void cancel_clicked();


private:
	PIMPL(GUI_DateSearchConfig)

	Ui::GUI_DateSearchConfig*	 ui=nullptr;

private:
	void commit();
};

#endif // GUI_DATESEARCHCONFIG_H
