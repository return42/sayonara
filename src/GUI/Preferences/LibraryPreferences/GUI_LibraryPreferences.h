/* GUI_LibraryPreferences.h */

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

#ifndef GUI_LIBRARYPREFERENCES_H
#define GUI_LIBRARYPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "Helper/Pimpl.h"

namespace Ui { class GUI_LibraryPreferences; }

class GUI_LibraryPreferences :
		public PreferenceWidgetInterface
{
	Q_OBJECT
	PIMPL(GUI_LibraryPreferences)

public:
	explicit GUI_LibraryPreferences(QWidget* parent=nullptr);
	virtual ~GUI_LibraryPreferences();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;

protected:
	void init_ui() override;
	void retranslate_ui() override;

private slots:
	void new_clicked();
	void delete_clicked();
	void add_clicked();
	void clear_clicked();
	void library_text_changed(const QString& string);

private:
	Ui::GUI_LibraryPreferences*	ui=nullptr;
};

#endif // GUI_LIBRARYPREFERENCES_H
