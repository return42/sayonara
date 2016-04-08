
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



#include "GUI_PlaylistEntryLook.h"

GUI_PlaylistEntryLook::GUI_PlaylistEntryLook(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::GUI_PlaylistLook()
{
	setupUi(this);

	revert();

	connect(btn_ok, &QPushButton::clicked, this, &GUI_PlaylistEntryLook::commit);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_PlaylistEntryLook::revert_and_close);
	connect(btn_default, &QPushButton::clicked, [=](){
		le_expression->setText("*%title%* - %artist%");
	});
}

void GUI_PlaylistEntryLook::commit()
{
	_settings->set(Set::PL_EntryLook, le_expression->text());
	close();
}

void GUI_PlaylistEntryLook::revert()
{
	le_expression->setText(_settings->get(Set::PL_EntryLook));
}

void GUI_PlaylistEntryLook::revert_and_close(){
	revert();
	close();
}

void GUI_PlaylistEntryLook::language_changed()
{
	retranslateUi(this);
}
