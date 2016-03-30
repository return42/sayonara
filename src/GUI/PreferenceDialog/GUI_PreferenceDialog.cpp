#include "GUI_PreferenceDialog.h"
#include <QLayout>

GUI_PreferenceDialog::GUI_PreferenceDialog(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_PreferenceDialog()
{
	setup_parent(this);

	widget_prefere->setLayout(new QVBoxLayout(widget_prefere));

	connect(list_preferences, &QListWidget::currentRowChanged, this, &GUI_PreferenceDialog::row_changed);
	connect(btn_ok, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_PreferenceDialog::revert);
}

GUI_PreferenceDialog::~GUI_PreferenceDialog()
{

}

void GUI_PreferenceDialog::register_preference_dialog(PreferenceDialogInterface* dialog)
{
	_dialogs << dialog;
	list_preferences->addItem(dialog->get_action_name());
}

void GUI_PreferenceDialog::language_changed()
{

}



QString GUI_PreferenceDialog::get_action_name() const
{
	return tr("Preferences");
}

QLabel* GUI_PreferenceDialog::get_title_label()
{
	return lab_title;
}

void GUI_PreferenceDialog::init_ui()
{



	show();
}

void GUI_PreferenceDialog::commit()
{
	for(PreferenceDialogInterface* iface : _dialogs){
		if(iface->is_ui_initialized()){
			iface->commit();
		}
	}

	close();
}

void GUI_PreferenceDialog::revert()
{
	for(PreferenceDialogInterface* iface : _dialogs){
		if(iface->is_ui_initialized()){
			iface->revert();
		}
	}

	close();
}

void GUI_PreferenceDialog::row_changed(int row)
{
	if(!between(row, 0, _dialogs.size())){
		return;
	}

	hide_all();
	QWidget* widget = _dialogs[row];
	widget_prefere->layout()->addWidget(widget);
	widget_prefere->layout()->setAlignment(Qt::AlignTop);

	widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	widget->show();

}

QWidget* GUI_PreferenceDialog::get_widget(){
	return widget_preferences;
}

void GUI_PreferenceDialog::hide_all(){

	for(PreferenceDialogInterface* iface : _dialogs){
		iface->setParent(nullptr);
		iface->hide();

	}
}
