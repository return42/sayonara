/* MenuButton.h */

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



#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include <QPushButton>
#include <QShowEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QMenu>
#include "Helper/SayonaraClass.h"


class MenuButton :
		public QPushButton,
		protected SayonaraClass
{

	Q_OBJECT

signals:
	void sig_triggered(QPoint);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void mousePressEvent(QMouseEvent* e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;

	virtual void show_menu(QPoint pos);
	virtual bool prove_enabled();

	void set_std_icon();

public:
	MenuButton(QWidget* parent=nullptr);
	virtual ~MenuButton();

protected slots:
	void _sl_skin_changed();
};

#endif // MENUBUTTON_H
