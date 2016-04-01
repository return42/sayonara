#include "GUI_Shortcuts.h"
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "GUI_ShortcutEntry.h"

#define ADD_TO_MAP(x) _btn_le_map[btn_##x] = le_##x


GUI_Shortcuts::GUI_Shortcuts(QWidget* parent) :
	PreferenceDialogInterface(parent)
{
	_sch = ShortcutHandler::getInstance();
}


void GUI_Shortcuts::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this);

	this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	cb_test->setVisible(false);

	QList<Shortcut> shortcuts = _sch->get_shortcuts();
	for(const Shortcut& shortcut : shortcuts){
		GUI_ShortcutEntry* entry = new GUI_ShortcutEntry(shortcut);
		connect(entry, &GUI_ShortcutEntry::sig_test_pressed,
				this, &GUI_Shortcuts::test_pressed);

		layout_entries->addWidget(entry);
		_entries << entry;
	}

	connect(cb_test, &QCheckBox::toggled, cb_test, [=]()
	{
		if(cb_test->isChecked()){
			cb_test->setText(tr("Success"));
			QTimer::singleShot(2500, cb_test, SLOT(hide()));
		}
	});
}



QString GUI_Shortcuts::get_action_name() const
{
	return tr("Shortcuts");
}


QLabel* GUI_Shortcuts::get_title_label()
{
	return lab_title;
}



void GUI_Shortcuts::commit(){

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
	cb_test->setVisible(true);
	cb_test->setText(tr("Press shortcut") + ": " + sequences[0].toString(QKeySequence::NativeText));
	cb_test->setChecked(false);
	for(const QKeySequence& sequence : sequences){
		cb_test->setShortcut(sequence);
	}

	cb_test->setFocus();

}

void GUI_Shortcuts::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceDialogInterface::language_changed();
}
