#include "GUI_PreferenceDialog.h"
#include "GUI/Helper/ComboBoxDelegate/ComboBoxDelegate.cpp"
#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include <QLayout>
#include "Helper/Helper.h"

GUI_PreferenceDialog::GUI_PreferenceDialog(QWidget *parent) :
	PreferenceDialogInterface(parent),
	Ui::GUI_PreferenceDialog()
{
	setup_parent(this);

	combo_ips->setItemDelegate(new ComboBoxDelegate(combo_ips));

	QStringList ip_addresses = Helper::get_ip_addresses();
	for(const QString& ip : ip_addresses){
		combo_ips->addItem(ip);
	}

	connect(list_preferences, &QListWidget::currentRowChanged, this, &GUI_PreferenceDialog::row_changed);

	connect(btn_apply, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit);
	connect(btn_ok, &QPushButton::clicked, this, &GUI_PreferenceDialog::commit_and_close);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_PreferenceDialog::revert);
}

GUI_PreferenceDialog::~GUI_PreferenceDialog()
{

}

void GUI_PreferenceDialog::register_preference_dialog(PreferenceWidgetInterface* dialog)
{
	_dialogs << dialog;
	list_preferences->addItem(dialog->get_action_name());
}

void GUI_PreferenceDialog::language_changed()
{
	translate_action();

	if(!is_ui_initialized()){
		return;
	}

	retranslateUi(this);

	int i=0;
	for(PreferenceWidgetInterface* dialog : _dialogs){
		QListWidgetItem* item = list_preferences->item(i);
		item->setText(dialog->get_action_name());
		i++;
	}

	PreferenceDialogInterface::language_changed();
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

}

void GUI_PreferenceDialog::commit_and_close(){
	commit();
	close();
}

void GUI_PreferenceDialog::commit()
{
	for(PreferenceWidgetInterface* iface : _dialogs){
		if(iface->is_ui_initialized()){
			iface->commit();
		}
	}
}

void GUI_PreferenceDialog::revert()
{
	for(PreferenceWidgetInterface* iface : _dialogs){
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

	widget_preferences->layout()->addWidget(widget);
	widget_preferences->layout()->setAlignment(Qt::AlignTop);

	widget->show();

}

QWidget* GUI_PreferenceDialog::get_widget(){
	return widget_preferences;
}

void GUI_PreferenceDialog::hide_all(){

	for(PreferenceWidgetInterface* iface : _dialogs){
		iface->setParent(nullptr);
		iface->hide();

	}
}
