/* GUI_Shortcuts.h */

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

#ifndef GUI_SHORTCUTS_H
#define GUI_SHORTCUTS_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "Utils/Pimpl.h"

UI_FWD(GUI_Shortcuts)

/**
 * @brief The GUI_Shortcuts class
 * @ingroup Shortcuts
 */
class GUI_Shortcuts final:
		public PreferenceWidgetInterface
{
	Q_OBJECT
	UI_CLASS(GUI_Shortcuts)
    PIMPL(GUI_Shortcuts)

public:
	explicit GUI_Shortcuts(QWidget* parent=nullptr);
	~GUI_Shortcuts();

	void revert() override;
	void commit() override;

	QString get_action_name() const override;

protected:
	void init_ui() override;
	void retranslate_ui() override;

private slots:
	void test_pressed(const QList<QKeySequence>& sequences);
	void sequence_entered();
};

#endif // GUI_SHORTCUTS_H
