#ifndef GUI_LIBRARYPREFERENCES_H
#define GUI_LIBRARYPREFERENCES_H

#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"
#include "GUI/LibraryPreferences/ui_GUI_LibraryPreferences.h"

class GUI_LibraryPreferences :
		public PreferenceDialogInterface,
		protected Ui::GUI_LibraryPreferences
{
	friend class PreferenceDialogInterface;

	Q_OBJECT

public:
	GUI_LibraryPreferences(QWidget* parent=nullptr);
	void commit() override;
	void revert() override;


private:
	void init_ui() override;
	QString get_action_name() const override;
	QLabel* get_title_label() override;


private slots:
	void language_changed() override;
};

#endif // GUI_LIBRARYPREFERENCES_H
