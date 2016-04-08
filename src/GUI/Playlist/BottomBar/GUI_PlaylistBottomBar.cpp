
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


#include "GUI_PlaylistBottomBar.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GlobalMessage/Message.h"
#include "GUI/Playlist/PlaylistMenu.h"
#ifdef WITH_SHUTDOWN
	#include "GUI/ShutdownDialog/GUI_Shutdown.h"
#endif


GUI_PlaylistBottomBar::GUI_PlaylistBottomBar(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_PlaylistBottomBar()
{
	this->setupUi(this);

	_playlist_menu = new PlaylistMenu(this);
	btn_menu->setFlat(true);

#ifdef WITH_SHUTDOWN
	_ui_shutdown = new GUI_Shutdown(this);
#endif

	_plm = _settings->get(Set::PL_Mode);

	btn_rep1->setChecked(_plm.rep1);
	btn_append->setChecked(_plm.append);
	btn_repAll->setChecked(_plm.repAll);
	btn_dynamic->setChecked(_plm.dynamic);
	btn_shuffle->setChecked(_plm.shuffle);
	btn_gapless->setChecked(_plm.gapless);

#ifdef WITH_SHUTDOWN
	btn_shutdown->setVisible(Shutdown::getInstance()->is_running());
#else
	btn_shutdown->setVisible(false);
#endif

	connect(btn_rep1, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep1_checked);
	connect(btn_repAll, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep_all_checked);
	connect(btn_shuffle, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::shuffle_checked);
	connect(btn_dynamic, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(btn_append, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(btn_gapless, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);

	connect(btn_menu, &MenuButton::sig_triggered, this, &GUI_PlaylistBottomBar::btn_menu_pressed);

#ifdef WITH_SHUTDOWN
	connect(_playlist_menu, &PlaylistMenu::sig_shutdown, this, &GUI_PlaylistBottomBar::shutdown_toggled);
	connect(btn_shutdown, &QPushButton::toggled, this, &GUI_PlaylistBottomBar::shutdown_toggled);
	connect(_ui_shutdown, &GUI_Shutdown::sig_closed, this, &GUI_PlaylistBottomBar::shutdown_closed);
#endif

	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
}

GUI_PlaylistBottomBar::~GUI_PlaylistBottomBar(){

}

void GUI_PlaylistBottomBar::btn_menu_pressed(QPoint pos){

	pos.setY(pos.y() - 160);
	_playlist_menu->exec(pos);

}

void GUI_PlaylistBottomBar::rep1_checked(bool checked)
{
	if(checked){
		btn_repAll->setChecked(false);
		btn_shuffle->setChecked(false);
	}

	playlist_mode_changed();
}

void GUI_PlaylistBottomBar::rep_all_checked(bool checked)
{
	if(checked){
		btn_rep1->setChecked(false);
	}

	playlist_mode_changed();
}

void GUI_PlaylistBottomBar::shuffle_checked(bool checked){
	if(checked){
		btn_rep1->setChecked(false);
	}

	playlist_mode_changed();
}

// internal gui slot
void GUI_PlaylistBottomBar::playlist_mode_changed() {

	parentWidget()->setFocus();

	PlaylistMode plm;

	plm.append = btn_append->isChecked();
	plm.rep1 = btn_rep1->isChecked();
	plm.repAll = btn_repAll->isChecked();
	plm.shuffle = btn_shuffle->isChecked();
	plm.dynamic = btn_dynamic->isChecked();
	plm.gapless = btn_gapless->isChecked();

	if(plm == _plm){
		return;
	}

	_plm = plm;

	_settings->set(Set::PL_Mode, _plm);
	_settings->set(Set::Engine_Gapless, _plm.gapless);
}

// setting slot
void GUI_PlaylistBottomBar::_sl_playlist_mode_changed(){

	PlaylistMode plm = _settings->get(Set::PL_Mode);

	if(plm == _plm) return;

	_plm = plm;

	btn_append->setChecked(_plm.append);
	btn_rep1->setChecked(_plm.rep1);
	btn_repAll->setChecked(_plm.repAll);
	btn_shuffle->setChecked(_plm.shuffle);
	btn_dynamic->setChecked(_plm.dynamic);
	btn_gapless->setChecked(_plm.gapless);

	btn_rep1->setVisible(_plm.ui_rep1);
	btn_append->setVisible(_plm.ui_append);
	btn_repAll->setVisible(_plm.ui_repAll);
	btn_dynamic->setVisible(_plm.ui_dynamic);
	btn_shuffle->setVisible(_plm.ui_shuffle);
	btn_gapless->setVisible(_plm.ui_gapless);
}



void GUI_PlaylistBottomBar::check_dynamic_play_button() {

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.isEmpty() || !QFile::exists(lib_path)) {
		btn_dynamic->setToolTip(tr("Please set library path first"));
	}

	else{
		btn_dynamic->setToolTip(tr("Dynamic playing"));
	}
}


#ifdef WITH_SHUTDOWN

	void GUI_PlaylistBottomBar::shutdown_toggled(bool b){

		Shutdown* shutdown = Shutdown::getInstance();
		bool shutdown_is_running = shutdown->is_running();

		if(shutdown_is_running == b) {
			return;
		}

		if(b){
			_ui_shutdown->exec();
		}

		else{
			GlobalMessage::Answer answer = Message::question_yn(tr("Cancel shutdown?"));

			if(answer == GlobalMessage::Answer::Yes) {
				Shutdown::getInstance()->stop();
			}
		}

		shutdown_is_running = shutdown->is_running();

		btn_shutdown->setVisible(shutdown_is_running);
		btn_shutdown->setChecked(shutdown_is_running);
		_playlist_menu->set_shutdown(shutdown_is_running);
	}


	void GUI_PlaylistBottomBar::shutdown_closed(){

		bool b = Shutdown::getInstance()->is_running();
		btn_shutdown->setVisible(b);
		btn_shutdown->setChecked(b);
	}

#endif

void GUI_PlaylistBottomBar::set_playlist_type(Playlist::Type playlist_type){
	btn_append->setVisible(playlist_type == Playlist::Type::Std);
	btn_dynamic->setVisible(playlist_type == Playlist::Type::Std);
	btn_repAll->setVisible(playlist_type == Playlist::Type::Std);
	btn_shuffle->setVisible(playlist_type == Playlist::Type::Std);
	btn_rep1->setVisible(playlist_type == Playlist::Type::Std);
	btn_gapless->setVisible(playlist_type == Playlist::Type::Std);
}





