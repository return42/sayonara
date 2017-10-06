/* GUI_Shortcuts.cpp */

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

#include "GUI_Shortcuts.h"
#include "GUI_ShortcutEntry.h"
#include "GUI/Preferences/ui_GUI_Shortcuts.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"
#include "Helper/Language.h"

#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#define ADD_TO_MAP(x) _btn_le_map[btn_##x] = le_##x

GUI_Shortcuts::GUI_Shortcuts(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{
	_sch = ShortcutHandler::instance();
}

GUI_Shortcuts::~GUI_Shortcuts()
{
	if(ui)
	{
		delete ui; ui=nullptr;
	}
}


void GUI_Shortcuts::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this, &ui);

	this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	ui->cb_test->setVisible(false);

	QList<Shortcut> shortcuts = _sch->get_shortcuts();

	for(const Shortcut& shortcut : shortcuts){
		GUI_ShortcutEntry* entry = new GUI_ShortcutEntry(shortcut);

		connect(entry, &GUI_ShortcutEntry::sig_test_pressed,
				this, &GUI_Shortcuts::test_pressed);
		connect(entry, &GUI_ShortcutEntry::sig_sequence_entered,
				this, &GUI_Shortcuts::sequence_entered);

		ui->layout_entries->addWidget(entry);

		_entries << entry;
	}

	connect(ui->cb_test, &QCheckBox::toggled, ui->cb_test, [=]()
	{
		if(ui->cb_test->isChecked()){
			ui->cb_test->setText(Lang::get(Lang::Success));
			QTimer::singleShot(2500, ui->cb_test, SLOT(hide()));
		}
	});
}


QString GUI_Shortcuts::get_action_name() const
{
	return tr("Shortcuts");
}


void GUI_Shortcuts::commit()
{
	for(GUI_ShortcutEntry* entry : _entries){
		entry->commit();
	}
}


void GUI_Shortcuts::revert()
{
	for(GUI_ShortcutEntry* entry : _entries){
		entry->revert();
	}
}


void GUI_Shortcuts::test_pressed(const QList<QKeySequence>& sequences)
{
	ui->cb_test->setVisible(true);
	ui->cb_test->setText(tr("Press shortcut") + ": " + sequences[0].toString(QKeySequence::NativeText));
	ui->cb_test->setChecked(false);

	for(const QKeySequence& sequence : sequences){
		ui->cb_test->setShortcut(sequence);
	}

	ui->cb_test->setFocus();
}

void GUI_Shortcuts::sequence_entered()
{
	GUI_ShortcutEntry* entry = static_cast<GUI_ShortcutEntry*>(sender());
	QList<QKeySequence> sequences = entry->get_sequences();

	for(GUI_ShortcutEntry* lst_entry : _entries)
	{
		if(lst_entry == entry){
			continue;
		}

		QList<QKeySequence> saved_sequences = lst_entry->get_sequences();
		for(const QKeySequence& seq1 : sequences)
		{
			QString seq1_str = seq1.toString(QKeySequence::NativeText);

			for(const QKeySequence& seq2 : saved_sequences)
			{
				QString seq2_str = seq2.toString(QKeySequence::NativeText);
				if(seq1_str == seq2_str && !seq1_str.isEmpty()){
					entry->show_sequence_error();
					break;
				}
			}
		}
	}
}

void GUI_Shortcuts::retranslate_ui()
{
	ui->retranslateUi(this);
}

