#ifndef GUI_SHORTCUTS_H
#define GUI_SHORTCUTS_H


#include "GUI/Shortcuts/ui_GUI_Shortcuts.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"

#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"

#include <QPushButton>
#include <QLineEdit>
#include <QMap>
#include <QKeyEvent>

#include <tuple>

class GUI_ShortcutEntry;
class GUI_Shortcuts :
		public PreferenceDialogInterface,
		private Ui::GUI_Shortcuts
{
	Q_OBJECT

	friend class PreferenceDialogInterface;

public:
	GUI_Shortcuts(QWidget* parent=nullptr);

	QString get_action_name() const override;
	void init_ui() override;
	QLabel* get_title_label() override;


private:
	ShortcutHandler*			_sch = nullptr;
	QList<GUI_ShortcutEntry*>	_entries;



private:
	void language_changed() override;

private slots:

	void cancel_clicked();
	void ok_clicked();
	void test_pressed(const QKeySequence& sequence);

};

#endif // GUI_SHORTCUTS_H
