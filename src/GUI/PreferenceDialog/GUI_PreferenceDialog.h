#ifndef GUI_PreferenceDialog_H
#define GUI_PreferenceDialog_H

#include <QDialog>
#include <QVector>
#include "GUI/PreferenceDialog/ui_GUI_PreferenceDialog.h"
#include "GUI/Helper/SayonaraWidget.h"
#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"

#include <QSpacerItem>

class PreferenceWidgetInterface;

class GUI_PreferenceDialog :
		public PreferenceDialogInterface,
		private Ui::GUI_PreferenceDialog
{

	friend class PreferenceDialogInterface;
	friend class PreferenceInterface<SayonaraDialog>;

	Q_OBJECT

public:
	explicit GUI_PreferenceDialog(QWidget *parent = 0);
	~GUI_PreferenceDialog();

	QString get_action_name() const override;
	QLabel* get_title_label() override;
	void init_ui() override;

	void register_preference_dialog(PreferenceWidgetInterface* dialog);

	QWidget* get_widget();


protected slots:
	void language_changed() override;

	void commit_and_close();
	void commit() override;
	void revert() override;

	void row_changed(int row);

protected:
	void hide_all();

private:
	QVector<PreferenceWidgetInterface*> _dialogs;
};

#endif // GUI_PreferenceDialog_H
