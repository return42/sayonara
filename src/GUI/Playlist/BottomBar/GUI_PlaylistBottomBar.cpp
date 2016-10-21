
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
#include "Helper/Message/Message.h"
#include "GUI/Playlist/PlaylistMenu.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#ifdef WITH_SHUTDOWN
	#include "GUI/ShutdownDialog/GUI_Shutdown.h"
#endif

#include <QFile>


GUI_PlaylistBottomBar::GUI_PlaylistBottomBar(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_PlaylistBottomBar()
{
	this->setupUi(this);

	_playlist_menu = new PlaylistMenu(this);
	btn_menu->setFlat(true);
	btn_menu->set_show_title(false);

#ifdef Q_OS_WIN
	btn_menu->setVisible(false);
#endif

#ifdef WITH_SHUTDOWN
	_ui_shutdown = new GUI_Shutdown(this);
#endif

	_plm = _settings->get(Set::PL_Mode);

	btn_rep1->setChecked(Playlist::Mode::isActive(_plm.rep1()));
	btn_append->setChecked(Playlist::Mode::isActive(_plm.append()));
	btn_repAll->setChecked(Playlist::Mode::isActive(_plm.repAll()));
	btn_dynamic->setChecked(Playlist::Mode::isActive(_plm.dynamic()));
	btn_shuffle->setChecked(Playlist::Mode::isActive(_plm.shuffle()));
	btn_gapless->setChecked(Playlist::Mode::isActive(_plm.gapless())) ;
	
	btn_rep1->setEnabled(Playlist::Mode::isEnabled(_plm.rep1()));
	btn_append->setEnabled(Playlist::Mode::isEnabled(_plm.append()));
	btn_repAll->setEnabled(Playlist::Mode::isEnabled(_plm.repAll()));
	btn_dynamic->setEnabled(Playlist::Mode::isEnabled(_plm.dynamic()));
	btn_shuffle->setEnabled(Playlist::Mode::isEnabled(_plm.shuffle()));

	bool gapless_enabled =
			Playlist::Mode::isEnabled(_plm.gapless()) &&
			!_settings->get(Set::Engine_CrossFaderActive);

	btn_gapless->setEnabled(gapless_enabled) ;
	
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

GUI_PlaylistBottomBar::~GUI_PlaylistBottomBar() {}

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

	Playlist::Mode plm;

	plm.setAppend(btn_append->isChecked(), btn_append->isEnabled());
	plm.setRep1(btn_rep1->isChecked(), btn_rep1->isEnabled());
	plm.setRepAll(btn_repAll->isChecked(), btn_repAll->isEnabled());
	plm.setShuffle(btn_shuffle->isChecked(), btn_shuffle->isEnabled());
	plm.setDynamic(btn_dynamic->isChecked(), btn_dynamic->isEnabled());
	plm.setGapless(btn_gapless->isChecked(), btn_gapless->isEnabled());

	if(plm == _plm){
		return;
	}

	_plm = plm;

	_settings->set(Set::PL_Mode, _plm);
}

// setting slot
void GUI_PlaylistBottomBar::_sl_playlist_mode_changed(){

	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	if(plm == _plm) return;

	_plm = plm;

	btn_append->setChecked( Playlist::Mode::isActive(_plm.append()));
	btn_rep1->setChecked(Playlist::Mode::isActive(_plm.rep1()));
	btn_repAll->setChecked(Playlist::Mode::isActive(_plm.repAll()));
	btn_shuffle->setChecked(Playlist::Mode::isActive(_plm.shuffle()));
	btn_dynamic->setChecked(Playlist::Mode::isActive(_plm.dynamic()));
	btn_gapless->setChecked(Playlist::Mode::isActive(_plm.gapless()));

	btn_rep1->setEnabled(Playlist::Mode::isEnabled(_plm.rep1()));
	btn_append->setEnabled(Playlist::Mode::isEnabled(_plm.append()));
	btn_repAll->setEnabled(Playlist::Mode::isEnabled(_plm.repAll()));
	btn_dynamic->setEnabled(Playlist::Mode::isEnabled(_plm.dynamic()));
	btn_shuffle->setEnabled(Playlist::Mode::isEnabled(_plm.shuffle()));
	btn_gapless->setEnabled(Playlist::Mode::isEnabled(_plm.gapless()));
}


void GUI_PlaylistBottomBar::check_dynamic_play_button() {

	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.isEmpty() || !QFile::exists(lib_path)) {
		btn_dynamic->setToolTip(tr("Please set library path first"));
	}

	else{
		btn_dynamic->setToolTip(Lang::get(Lang::DynamicPlayback));
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
