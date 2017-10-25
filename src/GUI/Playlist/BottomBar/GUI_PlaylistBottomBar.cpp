
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
#include "GUI/Utils/IconLoader/IconLoader.h"
#include "Utils/Playlist/PlaylistMode.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"

#include "Utils/Message/Message.h"

#ifdef WITH_SHUTDOWN
	#include "GUI/ShutdownDialog/GUI_Shutdown.h"
#endif

// Think about CMake
#include "Components/Library/LibraryManager.h"

#include <QFile>

struct GUI_PlaylistBottomBar::Private
{
	Playlist::Mode		plm;

#ifdef WITH_SHUTDOWN
	GUI_Shutdown*		ui_shutdown=nullptr;
	Shutdown*			shutdown=nullptr;
#endif

	Private()
	{
		shutdown = Shutdown::instance();
	}
};

GUI_PlaylistBottomBar::GUI_PlaylistBottomBar(QWidget *parent) :
	Widget(parent)
{
	m = Pimpl::make<Private>();

	ui = new Ui::GUI_PlaylistBottomBar();
	ui->setupUi(this);

#ifdef WITH_SHUTDOWN
	m->ui_shutdown = new GUI_Shutdown(this);
#endif

	m->plm = _settings->get(Set::PL_Mode);

	ui->btn_rep1->setChecked(Playlist::Mode::isActive(m->plm.rep1()));
	ui->btn_append->setChecked(Playlist::Mode::isActive(m->plm.append()));
	ui->btn_repAll->setChecked(Playlist::Mode::isActive(m->plm.repAll()));
	ui->btn_dynamic->setChecked(Playlist::Mode::isActive(m->plm.dynamic()));
	ui->btn_shuffle->setChecked(Playlist::Mode::isActive(m->plm.shuffle()));
	ui->btn_gapless->setChecked(Playlist::Mode::isActive(m->plm.gapless()));
	ui->btn_shutdown->setVisible(false);

	bool crossfader_active = _settings->get(Set::Engine_CrossFaderActive);
	bool gapless_enabled = (Playlist::Mode::isEnabled(m->plm.gapless()) && !crossfader_active);

	ui->btn_gapless->setEnabled(gapless_enabled) ;

	connect(ui->btn_rep1, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep1_checked);
	connect(ui->btn_repAll, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::rep_all_checked);
	connect(ui->btn_shuffle, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::shuffle_checked);
	connect(ui->btn_dynamic, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(ui->btn_append, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);
	connect(ui->btn_gapless, &QPushButton::released, this, &GUI_PlaylistBottomBar::playlist_mode_changed);

	connect(ui->btn_shutdown, &QPushButton::clicked, this, &GUI_PlaylistBottomBar::shutdown_clicked);
	connect(m->shutdown, &Shutdown::sig_started, this, &GUI_PlaylistBottomBar::shutdown_started);
	connect(m->shutdown, &Shutdown::sig_stopped, this, &GUI_PlaylistBottomBar::shutdown_closed);

	Set::listen(Set::PL_Mode, this, &GUI_PlaylistBottomBar::s_playlist_mode_changed);

}

GUI_PlaylistBottomBar::~GUI_PlaylistBottomBar() {}

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

	if(plm == m->plm){
		return;
	}

	m->plm = plm;

	_settings->set(Set::PL_Mode, m->plm);
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
void GUI_PlaylistBottomBar::s_playlist_mode_changed()
{
	Playlist::Mode plm = _settings->get(Set::PL_Mode);

	if(plm == m->plm) {
		return;
	}

	m->plm = plm;

	ui->btn_append->setChecked( Playlist::Mode::isActive(m->plm.append()));
	ui->btn_rep1->setChecked(Playlist::Mode::isActive(m->plm.rep1()));
	ui->btn_repAll->setChecked(Playlist::Mode::isActive(m->plm.repAll()));
	ui->btn_shuffle->setChecked(Playlist::Mode::isActive(m->plm.shuffle()));
	ui->btn_dynamic->setChecked(Playlist::Mode::isActive(m->plm.dynamic()));
	ui->btn_gapless->setChecked(Playlist::Mode::isActive(m->plm.gapless()));

	ui->btn_rep1->setEnabled(Playlist::Mode::isEnabled(m->plm.rep1()));
	ui->btn_append->setEnabled(Playlist::Mode::isEnabled(m->plm.append()));
	ui->btn_repAll->setEnabled(Playlist::Mode::isEnabled(m->plm.repAll()));
	ui->btn_dynamic->setEnabled(Playlist::Mode::isEnabled(m->plm.dynamic()));
	ui->btn_shuffle->setEnabled(Playlist::Mode::isEnabled(m->plm.shuffle()));
	ui->btn_gapless->setEnabled(Playlist::Mode::isEnabled(m->plm.gapless()));
}


void GUI_PlaylistBottomBar::check_dynamic_play_button()
{
	int n_libs = LibraryManager::instance()->count();

	/* TODO: */
	/* 	Dynamic playback */
	/* 	Use all artists from all libraries */

	if(n_libs > 0) {
		ui->btn_dynamic->setToolTip(tr("Please set library path first"));
	}

	else{
		ui->btn_dynamic->setToolTip(Lang::get(Lang::DynamicPlayback));
	}
}


#ifdef WITH_SHUTDOWN
	void GUI_PlaylistBottomBar::shutdown_clicked()
	{
		GlobalMessage::Answer answer = Message::question_yn(tr("Cancel shutdown?"));

		if(answer == GlobalMessage::Answer::Yes) {
			Shutdown::instance()->stop();
		}
	}

	void GUI_PlaylistBottomBar::shutdown_started(uint64_t time2go)
	{
		Q_UNUSED(time2go)

		bool b = Shutdown::instance()->is_running();
		ui->btn_shutdown->setVisible(b);
		ui->btn_shutdown->setChecked(b);
	}


	void GUI_PlaylistBottomBar::shutdown_closed()
	{
		bool b = Shutdown::instance()->is_running();
		ui->btn_shutdown->setVisible(b);
		ui->btn_shutdown->setChecked(b);
	}

#endif
