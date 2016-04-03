#ifndef GUI_SHORTCUTS_H
#define GUI_SHORTCUTS_H


#include "GUI/Shortcuts/ui_GUI_Shortcuts.h"
#include "GUI/Helper/Shortcuts/ShortcutHandler.h"

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

#include <QPushButton>
#include <QLineEdit>
#include <QMap>
#include <QKeyEvent>

#include <tuple>

class GUI_ShortcutEntry;
class GUI_Shortcuts :
		public PreferenceWidgetInterface,
		private Ui::GUI_Shortcuts
{
	Q_OBJECT

	friend class PreferenceWidgetInterface;
	friend class PreferenceInterface<SayonaraWidget>;

public:
	GUI_Shortcuts(QWidget* parent=nullptr);

	QString get_action_name() const override;
	QLabel* get_title_label() override;

	void revert() override;
	void commit() override;


private:
	ShortcutHandler*			_sch = nullptr;
	QList<GUI_ShortcutEntry*>	_entries;

private:
	void language_changed() override;
	void init_ui() override;

private slots:
	void test_pressed(const QList<QKeySequence>& sequences);


};

#endif // GUI_SHORTCUTS_H
