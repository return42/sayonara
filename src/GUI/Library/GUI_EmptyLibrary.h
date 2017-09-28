/* GUI_EmptyLibrary.h */

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



#ifndef GUI_EMPTYLIBRARY_H
#define GUI_EMPTYLIBRARY_H

#include "Helper/Pimpl.h"
#include <QComboBox>

namespace Ui
{
	class GUI_EmptyLibrary;
}

class QFrame;
class GUI_EmptyLibrary :
    public QWidget
{
	Q_OBJECT

public:
	explicit GUI_EmptyLibrary(QWidget* parent=nullptr);
	GUI_EmptyLibrary(const GUI_EmptyLibrary& other)  = delete;
	~GUI_EmptyLibrary();

	QFrame* header_frame() const;

private slots:
	void set_lib_path_clicked();

private:
	Ui::GUI_EmptyLibrary* ui=nullptr;
};

#endif // GUI_EMPTYLIBRARY_H
