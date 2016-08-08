/* PluginCloseButton.cpp */

#include "PluginCloseButton.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

PluginCloseButton::PluginCloseButton(QWidget *parent) :
	QPushButton(parent)
{
	this->setFlat(true);
	this->setIconSize(QSize(14,14));

	this->setStyleSheet("margin-left: 2px; margin-right: 2px; padding-left: 0px; padding-right: 0px; background: transparent;");
	this->setToolTip(tr("Close"));

	REGISTER_LISTENER(Set::Player_Style, _sl_skin_changed);
}

PluginCloseButton::~PluginCloseButton(){

}

void PluginCloseButton::mouseReleaseEvent(QMouseEvent *e){
	QPushButton::mouseReleaseEvent(e);
}


void PluginCloseButton::enterEvent(QEvent* e){

	QPushButton::enterEvent(e);

	bool dark = (_settings->get(Set::Player_Style) == 1);
	QIcon icon;

	if(dark){
		icon = GUI::get_icon("tool_grey");

	}

	else{
		icon = IconLoader::getInstance()->get_icon("window-close", "tool");
	}

	if( this->isEnabled() ){
		this->setIcon(icon);
		e->accept();
	}
}

void PluginCloseButton::leaveEvent(QEvent* e){
	QPushButton::leaveEvent(e);

	set_std_icon();
}


void PluginCloseButton::set_std_icon(){

	bool dark = (_settings->get(Set::Player_Style) == 1);

	QIcon icon;
	QPixmap pixmap;
	QPixmap pixmap_disabled;

	if(dark){

		pixmap = GUI::get_pixmap("tool_dark_grey");
		pixmap_disabled = GUI::get_pixmap("tool_disabled");
		icon.addPixmap(pixmap, QIcon::Normal, QIcon::On);
		icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
		icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::On);
		icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::Off);
		icon.addPixmap(pixmap, QIcon::Active, QIcon::On);
		icon.addPixmap(pixmap, QIcon::Active, QIcon::Off);
		icon.addPixmap(pixmap, QIcon::Selected, QIcon::On);
		icon.addPixmap(pixmap, QIcon::Selected, QIcon::Off);
	}

	else{
		icon = IconLoader::getInstance()->get_icon("window-close", "tool");
	}


	this->setIcon(icon);
	this->update();
}


void PluginCloseButton::_sl_skin_changed(){
	set_std_icon();
}

