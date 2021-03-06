/* Widget.h */

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

#ifndef SAYONARAWIDGET_H
#define SAYONARAWIDGET_H

#include "WidgetTemplate.h"
#include <QWidget>
#include <QMainWindow>

namespace Gui
{
	class Dialog;

	/**
	 * @brief Widget with Settings connection. Also contains triggers for language_changed() and skin_changed()
	 * \nThe widget's content can be exported to a Dialog via the box_into_dialog() method.
	 * @ingroup Widgets
	 * @ingroup Interfaces
	 */
	class Widget :
			public Gui::WidgetTemplate<QWidget>
	{
		Q_OBJECT

	signals:
		void sig_shown();
		void sig_closed();

	public:
		explicit Widget(QWidget* parent=nullptr);
		virtual ~Widget();

		Dialog* box_into_dialog();

	protected:
		Dialog* _boxed_dialog=nullptr;

		virtual void showEvent(QShowEvent* e);
		virtual void closeEvent(QCloseEvent* e);
	};

	/**
	 * @brief The SayonaraMainWindow class
	 * @ingroup Widgets
	 */
	class MainWindow :
			public Gui::WidgetTemplate<QMainWindow>
	{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent=nullptr);
		virtual ~MainWindow();

		void raise();
	};
}

#endif // SAYONARAWIDGET_H
