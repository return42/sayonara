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
	GUI_LibraryPreferences(QWidget* parent);


private:
	virtual void init_ui() override;
	virtual QString get_action_name() const override;
	virtual QLabel* get_title_label() override;


private slots:

	void ok_clicked();
	void cancel_clicked();

	void language_changed() override;
};

#endif // GUI_LIBRARYPREFERENCES_H
