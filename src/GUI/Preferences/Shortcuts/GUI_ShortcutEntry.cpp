
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

#include "GUI_ShortcutEntry.h"
#include "GUI/Preferences/ui_GUI_ShortcutEntry.h"
#include "GUI/Utils/IconLoader/IconLoader.h"
#include "GUI/Utils/Shortcuts/ShortcutHandler.h"
#include "Utils/Language.h"

#include <QKeySequence>
#include <QShortcut>
#include <QMessageBox>

GUI_ShortcutEntry::GUI_ShortcutEntry(const Shortcut& shortcut, QWidget* parent) :
	Widget(parent),
	_shortcut(shortcut)
{
	ui = new Ui::GUI_ShortcutEntry();
	ui->setupUi(this);

	_sch = ShortcutHandler::instance();

	ui->lab_description->setText(_shortcut.get_name());
	ui->le_entry->setText(_shortcut.get_shortcuts().join(", "));

	connect(ui->btn_edit, &QPushButton::clicked, this, &GUI_ShortcutEntry::edit_clicked);
	connect(ui->btn_default, &QPushButton::clicked, this, &GUI_ShortcutEntry::default_clicked);
	connect(ui->btn_test, &QPushButton::clicked, this, &GUI_ShortcutEntry::test_clicked);
	connect(ui->le_entry, &ShortcutLineEdit::sig_sequence_entered, this, &GUI_ShortcutEntry::sig_sequence_entered);

	skin_changed();
}

GUI_ShortcutEntry::~GUI_ShortcutEntry()
{
	if(ui){ delete ui; ui = nullptr; }
}

QList<QKeySequence> GUI_ShortcutEntry::get_sequences() const
{
	return ui->le_entry->get_sequences();
}

void GUI_ShortcutEntry::show_sequence_error()
{
	ui->le_entry->clear();
	QMessageBox::warning(this, Lang::get(Lang::Error), tr("Shortcut already in use"));
}

void GUI_ShortcutEntry::commit()
{
	QString identifier = _shortcut.get_identifier();

	_sch->set_shortcut(identifier, ui->le_entry->text().split(", "));
	_shortcut = _sch->get_shortcut(identifier);
}

void GUI_ShortcutEntry::clear()
{
	ui->le_entry->clear();
}

void GUI_ShortcutEntry::revert()
{
	ui->le_entry->setText(
		_shortcut.get_shortcuts().join(", ")
	);
}


void GUI_ShortcutEntry::default_clicked()
{
	ui->le_entry->setText(
		_shortcut.get_default().join(", ")
	);
}

void GUI_ShortcutEntry::test_clicked()
{	
	QStringList splitted = ui->le_entry->text().split(", ");
	QList<QKeySequence> sequences;

	for(const QString& str : splitted){
		sequences << QKeySequence::fromString(str, QKeySequence::NativeText);
	}

	emit sig_test_pressed(sequences);
}

void GUI_ShortcutEntry::language_changed()
{
	ui->retranslateUi(this);

	ui->lab_description->setText(_shortcut.get_name());

	ui->btn_default->setToolTip(Lang::get(Lang::Default));
	ui->btn_edit->setToolTip(Lang::get(Lang::Edit));
	ui->btn_test->setToolTip(tr("Test"));
}

void GUI_ShortcutEntry::skin_changed()
{
	IconLoader* icon_loader = IconLoader::instance();
	ui->btn_default->setIcon(icon_loader->icon("undo", "undo"));
	ui->btn_edit->setIcon(icon_loader->icon("accessories-text-editor", "edit"));
	ui->btn_test->setIcon(icon_loader->icon("dialog-info", "info"));
}


void GUI_ShortcutEntry::edit_clicked()
{
	ui->le_entry->clear();
	ui->le_entry->setFocus();
}

