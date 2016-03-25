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

	cb_test->setVisible(false);

	QList<Shortcut> shortcuts = _sch->get_shortcuts();
	for(const Shortcut& shortcut : shortcuts){
		GUI_ShortcutEntry* entry = new GUI_ShortcutEntry(shortcut);
		connect(entry, &GUI_ShortcutEntry::sig_test_pressed,
				this, &GUI_Shortcuts::test_pressed);

		layout_entries->addWidget(entry);
		_entries << entry;
	}

	connect(btn_ok, &QPushButton::clicked, this, &GUI_Shortcuts::ok_clicked);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_Shortcuts::cancel_clicked);


	connect(cb_test, &QCheckBox::toggled, cb_test, [=]()
	{
		if(cb_test->isChecked()){
			cb_test->setText(tr("Success"));
			QTimer::singleShot(2500, cb_test, SLOT(hide()));
		}
	});
}


void GUI_Shortcuts::cancel_clicked()
{
	this->close();
}

void GUI_Shortcuts::ok_clicked(){

	for(GUI_ShortcutEntry* entry : _entries){
		entry->commit();
	}

	this->close();
}

void GUI_Shortcuts::test_pressed(const QKeySequence& sequence)
{
	cb_test->setVisible(true);
	cb_test->setText(tr("Press shortcut"));
	cb_test->setChecked(false);
	cb_test->setShortcut(sequence);
	cb_test->setFocus();

}

QString GUI_Shortcuts::get_action_name() const
{
	return tr("Shortcuts");
}


QLabel* GUI_Shortcuts::get_title_label()
{
	return lab_title;
}

void GUI_Shortcuts::language_changed()
{
	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);
	PreferenceDialogInterface::language_changed();
}

