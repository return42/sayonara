/* MenuButton.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#include "MenuButton.h"

MenuButton::MenuButton(QWidget* parent) :
	QPushButton(parent),
	SayonaraClass()
{
	this->setFlat(true);
	this->setIconSize(QSize(16,16));

	REGISTER_LISTENER(Set::Player_Style, _sl_skin_changed);
}

MenuButton::~MenuButton()
{

}


void MenuButton::show_menu(QPoint pos){
	Q_UNUSED(pos)
	this->setAttribute( Qt::WA_Hover, false);
	this->setAttribute( Qt::WA_UnderMouse, false);
	return;
}

bool MenuButton::prove_enabled(){
	return true;
}


void MenuButton::mousePressEvent(QMouseEvent* e){

	QPushButton::mousePressEvent(e);

	QPoint globalPoint = this->mapToGlobal(this->pos()) - this->pos();

	emit sig_triggered(globalPoint);

	show_menu(globalPoint);
}

void MenuButton::mouseReleaseEvent(QMouseEvent *e){
	QPushButton::mouseReleaseEvent(e);
}


void MenuButton::enterEvent(QEvent* e){

	QPushButton::enterEvent(e);

	bool dark = (_settings->get(Set::Player_Style) == 1);
	QIcon icon;

	if(dark){
		icon = Helper::get_icon("tool_grey");

	}

	else{
		icon = Helper::get_icon("tool");
	}

	if( this->isEnabled() ){
		this->setIcon(icon);
		e->accept();
	}
}

void MenuButton::leaveEvent(QEvent* e){
	QPushButton::leaveEvent(e);

	set_std_icon();
}


void MenuButton::set_std_icon(){

	bool dark = (_settings->get(Set::Player_Style) == 1);

	QIcon icon;
	QPixmap pixmap;
	QPixmap pixmap_disabled;

	if(dark){

		pixmap = Helper::get_pixmap("tool_dark_grey");
		pixmap_disabled = Helper::get_pixmap("tool_disabled");
	}

	else{
		pixmap = Helper::get_pixmap("tool");
		pixmap_disabled = Helper::get_pixmap("tool");
	}

	icon.addPixmap(pixmap, QIcon::Normal, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
	icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::On);
	icon.addPixmap(pixmap_disabled, QIcon::Disabled, QIcon::Off);
	icon.addPixmap(pixmap, QIcon::Active, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Active, QIcon::Off);
	icon.addPixmap(pixmap, QIcon::Selected, QIcon::On);
	icon.addPixmap(pixmap, QIcon::Selected, QIcon::Off);


	this->setIcon(icon);
	this->update();
}


void MenuButton::_sl_skin_changed(){
	set_std_icon();
}



