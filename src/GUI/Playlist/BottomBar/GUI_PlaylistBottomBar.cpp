
/* Copyright (C) 2011-2017  Lucio Carreras
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
#include "GUI/Playlist/ui_GUI_PlaylistBottomBar.h"
#include "GUI/Playlist/PlaylistMenu.h"
#include "GUI/Helper/IconLoader/IconLoader.h"

#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include "Helper/Message/Message.h"

#ifdef WITH_SHUTDOWN
	#include "GUI/ShutdownDialog/GUI_Shutdown.h"
#endif

#include <QFile>

struct GUI_PlaylistBottomBar::Private
{
	Playlist::Mode		plm;
	PlaylistMenu*		playlist_menu=nullptr;

#ifdef WITH_SHUTDOWN
	GUI_Shutdown*		ui_shutdown=nullptr;
#endif
};

GUI_PlaylistBottomBar::GUI_PlaylistBottomBar(QWidget *parent) :
	SayonaraWidget(parent)
{
	_m = Pimpl::make<Private>();
	_m->playlist_menu = new PlaylistMenu(this);

	ui = new Ui::GUI_PlaylistBottomBar();
	ui->setupUi(this);

	ui->btn_menu->setFlat(true);
	ui->btn_menu->set_show_title(false);

#ifdef Q_OS_WIN
	ui->btn_menu->setVisible(false);
#endif

#ifdef WITH_SHUTDOWN
	_m->ui_shutdown = new GUI_Shutdown(this);
#endif

	_m->plm = _settings->get(Set::PL_Mode);

	ui->btn_rep1->setChecked(Playlist::Mode::isActive(_m->plm.rep1()));
	ui->btn_append->setChecked(Playlist::Mode::isActive(_m->plm.append()));
	ui->btn_repAll->setChecked(Playlist::Mode::isActive(_m->plm.repAll()));
	ui->btn_dynamic->setChecked(Playlist::Mode::isActive(_m->plm.dynamic()));
	ui->btn_shuffle->setChecked(Playlist::Mode::isActive(_m->plm.shuffle()));
	ui->btn_gapless->setChecked(Playlist::Mode::isActive(_m->plm.gapless())) ;
	
	ui->btn_rep1->setEnabled(Playlist::Mode::isEnabled(_m->plm.rep1()));
	ui->btn_append->setEnabled(Playlist::Mode::isEnabled(_m->plm.append()));
	ui->btn_repAll->setEnabled(Playlist::Mode::isEnabled(_m->plm.repAll()));
	ui->btn_dynamic->setEnabled(Playlist::Mode::isEnabled(_m->plm.dynamic()));
	ui->btn_shuffle->setEnabled(Playlist::Mode::isEnabled(_m->plm.shuffle()));

	bool gapless_enabled =
			Playlist::Mode::isEnabled(_m->plm.gapless()) &&
			!_settings->get(Set::Engine_CrossFaderActive);

	ui->btn_gapless->setEnabled(gapless_enabled) ;
	
#ifdef WITH_SHUTDOWN
	ui->btn_shutdown->setVisible(Shutdown::getInstance()->is_running());
#else
	ui->btn_shutdown->setVisible(false);
#endif

	connect(ui->btn_rep1, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep1_checked);
	connect(ui->btn_repAll, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep_all_checked);
	connect(ui->btn_shuffle, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::shuffle_checked);
	connect(ui->btn_dynamic, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(ui->btn_append, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(ui->btn_gapless, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);

	connect(ui->btn_menu, &MenuButton::sig_triggered, this, &GUI_PlaylistBottomBar::btn_menu_pressed);

#ifdef WITH_SHUTDOWN
	connect(_m->playlist_menu, &PlaylistMenu::sig_shutdown, this, &GUI_PlaylistBottomBar::shutdown_toggled);
	connect(ui->btn_shutdown, &QPushButton::toggled, this, &GUI_PlaylistBottomBar::shutdown_toggled);
	connect(_m->ui_shutdown, &GUI_Shutdown::sig_closed, this, &GUI_PlaylistBottomBar::shutdown_closed);
#endif

	language_changed();
	REGISTER_LISTENER(Set::PL_Mode, _sl_playlist_mode_changed);
}

GUI_PlaylistBottomBar::~GUI_PlaylistBottomBar() {}

void GUI_PlaylistBottomBar::btn_menu_pressed(QPoint pos)
{
	pos.setY(pos.y() - 160);
	_m->playlist_menu->exec(pos);
}

void GUI_PlaylistBottomBar::rep1_checked(bool checked)
{
	if(checked){
		ui->btn_repAll->setChecked(false);
		ui->btn_shuffle->setChecked(false);
	}

	playlist_mode_changed();
}

void GUI_PlaylistBottomBar::rep_all_checked(bool checked)
{
	if(checked){
		ui->btn_rep1->setChecked(false);
	}

	playlist_mode_changed();
}

void GUI_PlaylistBottomBar::shuffle_checked(bool checked)
{
	if(checked){
		ui->btn_rep1->setChecked(false);
	}

	playlist_mode_changed();
}

// internal gui slot
void GUI_PlaylistBottomBar::playlist_mode_changed()
{
	parentWidget()->setFocus();

	Playlist::Mode plm;

	plm.setAppend(ui->btn_append->isChecked(), ui->btn_append->isEnabled());
	plm.setRep1(ui->btn_rep1->isChecked(), ui->btn_rep1->isEnabled());
	plm.setRepAll(ui->btn_repAll->isChecked(), ui->btn_repAll->isEnabled());
	plm.setShuffle(ui->btn_shuffle->isChecked(), ui->btn_shuffle->isEnabled());
	plm.setDynamic(ui->btn_dynamic->isChecked(), ui->btn_dynamic->isEnabled());
	plm.setGapless(ui->btn_gapless->isChecked(), ui->btn_gapless->isEnabled());

	if(plm == _m->plm){
		return;
	}

	_m->plm = plm;

	_settings->set(Set::PL_Mode, _m->plm);
}

void GUI_PlaylistBottomBar::language_changed()
{
	if(!ui){
		return;
	}

	ui->btn_append->setToolTip(Lang::get(Lang::Append));
	ui->btn_dynamic->setToolTip(Lang::get(Lang::DynamicPlayback));
	ui->btn_gapless->setToolTip(Lang::get(Lang::GaplessPlayback));
	ui->btn_rep1->setToolTip(Lang::get(Lang::Repeat1));
	ui->btn_repAll->setToolTip(Lang::get(Lang::RepeatAll));
	ui->btn_shuffle->setToolTip(Lang::get(Lang::Shuffle));
	ui->btn_shutdown->setToolTip(Lang::get(Lang::Shutdown) + ": " + Lang::get(Lang::Cancel));
}

// setting slot
void GUI_PlaylistBottomBar::_sl_playlist_mode_changed()
{
	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	if(plm == _m->plm) return;

	_m->plm = plm;

	ui->btn_append->setChecked( Playlist::Mode::isActive(_m->plm.append()));
	ui->btn_rep1->setChecked(Playlist::Mode::isActive(_m->plm.rep1()));
	ui->btn_repAll->setChecked(Playlist::Mode::isActive(_m->plm.repAll()));
	ui->btn_shuffle->setChecked(Playlist::Mode::isActive(_m->plm.shuffle()));
	ui->btn_dynamic->setChecked(Playlist::Mode::isActive(_m->plm.dynamic()));
	ui->btn_gapless->setChecked(Playlist::Mode::isActive(_m->plm.gapless()));

	ui->btn_rep1->setEnabled(Playlist::Mode::isEnabled(_m->plm.rep1()));
	ui->btn_append->setEnabled(Playlist::Mode::isEnabled(_m->plm.append()));
	ui->btn_repAll->setEnabled(Playlist::Mode::isEnabled(_m->plm.repAll()));
	ui->btn_dynamic->setEnabled(Playlist::Mode::isEnabled(_m->plm.dynamic()));
	ui->btn_shuffle->setEnabled(Playlist::Mode::isEnabled(_m->plm.shuffle()));
	ui->btn_gapless->setEnabled(Playlist::Mode::isEnabled(_m->plm.gapless()));
}


void GUI_PlaylistBottomBar::check_dynamic_play_button()
{
	QString lib_path = _settings->get(Set::Lib_Path);

	if(lib_path.isEmpty() || !QFile::exists(lib_path)) {
		ui->btn_dynamic->setToolTip(tr("Please set library path first"));
	}

	else{
		ui->btn_dynamic->setToolTip(Lang::get(Lang::DynamicPlayback));
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
			_m->ui_shutdown->exec();
		}

		else{
			GlobalMessage::Answer answer = Message::question_yn(tr("Cancel shutdown?"));

			if(answer == GlobalMessage::Answer::Yes) {
				Shutdown::getInstance()->stop();
			}
		}

		shutdown_is_running = shutdown->is_running();

		ui->btn_shutdown->setVisible(shutdown_is_running);
		ui->btn_shutdown->setChecked(shutdown_is_running);
		_m->playlist_menu->set_shutdown(shutdown_is_running);
	}


	void GUI_PlaylistBottomBar::shutdown_closed()
	{
		bool b = Shutdown::getInstance()->is_running();
		ui->btn_shutdown->setVisible(b);
		ui->btn_shutdown->setChecked(b);
	}

#endif
