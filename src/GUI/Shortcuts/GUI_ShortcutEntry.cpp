
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



#include "GUI_ShortcutEntry.h"
#include <QKeySequence>
#include <QShortcut>

GUI_ShortcutEntry::GUI_ShortcutEntry(const Shortcut& shortcut, QWidget* parent) :
	SayonaraWidget(parent),
	Ui::GUI_ShortcutEntry(),
	_shortcut(shortcut)

{
	setupUi(this);


	_sch = ShortcutHandler::getInstance();

	lab_description->setText(_shortcut.get_name());
	le_entry->setText(_shortcut.get_shortcuts().join(", "));

	connect(btn_edit, &QPushButton::clicked, this, &GUI_ShortcutEntry::edit_clicked);
	//connect(btn_clear, &QPushButton::clicked, this, &GUI_ShortcutEntry::clear);
	connect(btn_default, &QPushButton::clicked, this, &GUI_ShortcutEntry::default_clicked);
	connect(btn_test, &QPushButton::clicked, this, &GUI_ShortcutEntry::test_clicked);
}

void GUI_ShortcutEntry::commit()
{
	QString identifier = _shortcut.get_identifier();

	_sch->set_shortcut(identifier, le_entry->text().split(", "));
	_shortcut = _sch->get_shortcut(identifier);
}

void GUI_ShortcutEntry::clear(){
	le_entry->setText("");
}

void GUI_ShortcutEntry::revert(){
	le_entry->setText(_shortcut.get_shortcuts().join(", "));
}


void GUI_ShortcutEntry::default_clicked(){
	le_entry->setText(_shortcut.get_default().join(", "));
}

void GUI_ShortcutEntry::test_clicked()
{	
	QStringList splitted = le_entry->text().split(", ");
	QList<QKeySequence> sequences;

	for(const QString& str : splitted){
		sequences << QKeySequence::fromString(str, QKeySequence::NativeText);
	}

	emit sig_test_pressed(sequences);

}



void GUI_ShortcutEntry::edit_clicked(){
	le_entry->clear();
	le_entry->setFocus();
}
