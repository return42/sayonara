/* SayonaraWidget.cpp */

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

#include "SayonaraWidget.h"
#include <QFontMetrics>

SayonaraWidget::SayonaraWidget(QWidget* parent) :
	SayonaraWidgetTemplate<QWidget>(parent)
{
	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

SayonaraWidget::~SayonaraWidget(){}

void SayonaraWidget::language_changed(){}
void SayonaraWidget::skin_changed(){}



SayonaraDialog::SayonaraDialog(QWidget* parent) :
	SayonaraWidgetTemplate<QDialog>(parent)
{
	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

SayonaraDialog::~SayonaraDialog(){}

void SayonaraDialog::language_changed(){}
void SayonaraDialog::skin_changed(){}


SayonaraMainWindow::SayonaraMainWindow(QWidget* parent) :
	SayonaraWidgetTemplate<QMainWindow>(parent)
{
	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER(Set::Player_Style, skin_changed);
}

SayonaraMainWindow::~SayonaraMainWindow(){}

void SayonaraMainWindow::language_changed(){}
void SayonaraMainWindow::skin_changed(){}
