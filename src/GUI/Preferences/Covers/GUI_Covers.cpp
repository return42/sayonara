#include "GUI_Covers.h"
#include "GUI/Preferences/ui_GUI_Covers.h"
#include "Components/Covers/CoverFetchManager.h"
#include "Components/Covers/CoverFetcherInterface.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include <QListWidgetItem>
#include <QList>

GUI_Covers::GUI_Covers(QWidget* parent) :
	PreferenceWidgetInterface (parent) {}


GUI_Covers::~GUI_Covers()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}

void GUI_Covers::commit()
{
	Settings* settings = Settings::getInstance();
	QStringList active_items;
	for(int i=0; i<ui->lv_active->count(); i++){
		QListWidgetItem* item = ui->lv_active->item(i);
		active_items << item->text();
	}

	settings->set(Set::Cover_Server, active_items);
}

void GUI_Covers::revert()
{
	Settings* settings = Settings::getInstance();
	QStringList active = settings->get(Set::Cover_Server);

	CoverFetchManager* cfm = CoverFetchManager::getInstance();

	QList<CoverFetcherInterface*> cfis = cfm->get_available_coverfetchers();
	for(const CoverFetcherInterface* cfi : cfis) {
		if(cfi->get_keyword().isEmpty()){
			continue;
		}

		if(active.contains(cfi->get_keyword())){
			ui->lv_active->addItem(cfi->get_keyword());
		}

		else{
			ui->lv_inactive->addItem(cfi->get_keyword());
		}
	}
}

QString GUI_Covers::get_action_name() const
{
	return Lang::get(Lang::Covers);
}


void GUI_Covers::init_ui()
{
	if(ui){
		return;
	}

	setup_parent(this, &ui);
	ui->lv_active->clear();
	ui->lv_inactive->clear();

	revert();
}

void GUI_Covers::retranslate_ui()
{
	ui->retranslateUi(this);
}
