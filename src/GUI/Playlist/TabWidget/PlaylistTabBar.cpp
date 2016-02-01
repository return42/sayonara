/* PlaylistTabBar.cpp */

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



#include "PlaylistTabBar.h"
#include "PlaylistTabMenu.h"


PlaylistTabBar::PlaylistTabBar(QWidget *parent) :
	QTabBar(parent)
{
	_menu = new PlaylistTabMenu(this);
	this->setDrawBase(false);

	connect(_menu, &PlaylistTabMenu::sig_rename_clicked, this, &PlaylistTabBar::rename_pressed);
	connect(_menu, &PlaylistTabMenu::sig_reset_clicked, this, &PlaylistTabBar::reset_pressed);
	connect(_menu, &PlaylistTabMenu::sig_save_clicked, this, &PlaylistTabBar::save_pressed);
	connect(_menu, &PlaylistTabMenu::sig_save_as_clicked, this, &PlaylistTabBar::save_as_pressed);
	connect(_menu, &PlaylistTabMenu::sig_clear_clicked, this, &PlaylistTabBar::clear_pressed);
	connect(_menu, &PlaylistTabMenu::sig_delete_clicked, this, &PlaylistTabBar::delete_pressed);
	connect(_menu, &PlaylistTabMenu::sig_close_clicked, this, &PlaylistTabBar::close_pressed);
	connect(_menu, &PlaylistTabMenu::sig_close_others_clicked, this, &PlaylistTabBar::close_others_pressed);
}


PlaylistTabBar::~PlaylistTabBar(){
	delete _menu;
}


void PlaylistTabBar::save_pressed(){
	emit sig_tab_save(currentIndex());
}

void PlaylistTabBar::save_as_pressed(){
	int cur_idx = currentIndex();
	QString cur_text = tabText(cur_idx);

	QString name = QInputDialog::getText(
				this,
				tr("Save as..."),
				cur_text + ": " + tr("Save as"));

	emit sig_tab_save_as(currentIndex(), name);
}

void PlaylistTabBar::clear_pressed()
{
	emit sig_tab_clear(currentIndex());
}

void PlaylistTabBar::delete_pressed(){
	emit sig_tab_delete(currentIndex());
}

void PlaylistTabBar::close_pressed(){
	emit tabCloseRequested(this->currentIndex());
}

void PlaylistTabBar::reset_pressed(){
	emit sig_tab_reset(currentIndex());
}

void PlaylistTabBar::rename_pressed(){
	int cur_idx = currentIndex();
	QString cur_text = tabText(cur_idx);

	QString name = QInputDialog::getText(
				this,
				tr("Rename"),
				cur_text + ": " + tr("Rename"));

	if(name.compare(cur_text) == 0){
		return;
	}

	emit sig_tab_rename(currentIndex(), name);
}

void PlaylistTabBar::close_others_pressed(){
	int my_tab = currentIndex();
	int i=0;

	while( count() > 2){
		if(i < my_tab){
			tabCloseRequested(0);
		}

		else if(i == my_tab){

		}

		else{
			tabCloseRequested(1);
		}

		i++;
	}
}

void PlaylistTabBar::mousePressEvent(QMouseEvent* e){

	int idx = this->tabAt(e->pos());

	if(idx == this->count() - 1){
		emit sig_add_tab_clicked();
		return;
	}

	else{
		this->setCurrentIndex(idx);
	}

	if(e->button() == Qt::RightButton){
		_menu->exec(e->globalPos());
	}
}

void PlaylistTabBar::wheelEvent(QWheelEvent* e)
{
	QTabBar::wheelEvent(e);
	if(this->currentIndex() == this->count() - 1 &&
	   this->count() > 1)
	{
		this->setCurrentIndex(this->count() - 2);
	}
}


void PlaylistTabBar::show_menu_items(PlaylistMenuEntries entries){
	_menu->show_menu_items(entries);
}

void PlaylistTabBar::setTabsClosable(bool b){
	QTabBar::setTabsClosable(b);
	_menu->show_close(b);
}
