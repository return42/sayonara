#ifndef GUI_PREFERENCEDIALOG_H
#define GUI_PREFERENCEDIALOG_H

#include <QDialog>
#include <QVector>
#include "GUI/PreferenceDialog/ui_GUI_PreferenceDialog.h"
#include "GUI/Helper/SayonaraWidget.h"
#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"
#include <QSpacerItem>
class PreferenceDialogInterface;

class GUI_PreferenceDialog :
		public PreferenceDialogInterface,
		private Ui::GUI_PreferenceDialog
{

	friend class PreferenceDialogInterface;

	Q_OBJECT

public:
	explicit GUI_PreferenceDialog(QWidget *parent = 0);
	~GUI_PreferenceDialog();

	QString get_action_name() const override;
	QLabel* get_title_label() override;
	void init_ui() override;

	void register_preference_dialog(PreferenceDialogInterface* dialog);

	QWidget* get_widget();


protected slots:
	void language_changed() override;
	void commit() override;
	void revert() override;

	void row_changed(int row);

protected:
	void hide_all();

private:
	QVector<PreferenceDialogInterface*> _dialogs;
};

#endif // GUI_PREFERENCEDIALOG_H
