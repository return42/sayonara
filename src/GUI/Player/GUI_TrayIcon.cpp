/* GUI_TrayIcon.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras  gleugner
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

#include "GUI_TrayIcon.h"

#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/Style/Style.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Language.h"

#include "Components/PlayManager/PlayManager.h"

#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QWheelEvent>
#include <QHoverEvent>
#include <QIcon>
#include <QTimer>


GUI_TrayIcon::GUI_TrayIcon (QObject *parent) :
	QSystemTrayIcon(parent),
    NotificationInterface("Standard")
{
	_settings = Settings::getInstance();
	_play_manager = PlayManager::getInstance();

	connect(_play_manager, &PlayManager::sig_playstate_changed, this, &GUI_TrayIcon::playstate_changed);
	connect(_play_manager, &PlayManager::sig_mute_changed, this, &GUI_TrayIcon::mute_changed);

	init_context_menu();

	playstate_changed(_play_manager->playstate());

	bool muted = _play_manager->is_muted();
	mute_changed(muted);

	Set::listen(Set::Player_Language, this, &GUI_TrayIcon::language_changed);
	Set::listen(Set::Player_Style, this, &GUI_TrayIcon::skin_changed);

	NotificationHandler::getInstance()->register_notificator(this);
}

GUI_TrayIcon::~GUI_TrayIcon() {}

void GUI_TrayIcon::language_changed()
{
	_play_action->setText(Lang::get(Lang::PlayPause));
	_fwd_action->setText(Lang::get(Lang::NextTrack));
	_bwd_action->setText(Lang::get(Lang::PreviousTrack));
	_stop_action->setText(Lang::get(Lang::Stop));
	_mute_action->setText(Lang::get(Lang::MuteOn));
	_close_action->setText(Lang::get(Lang::Close));
	_show_action->setText(Lang::get(Lang::Show));
	_cur_song_action->setText(tr("Current song"));
}

void GUI_TrayIcon::init_context_menu()
{
	if(_context_menu){
		return;
	}

	_play_action = new QAction(Lang::get(Lang::PlayPause), this);
	_play_action->setIcon(GUI::get_icon("play"));
	_stop_action = new QAction(Lang::get(Lang::Stop), this);
	_stop_action->setIcon(GUI::get_icon("stop"));
	_bwd_action = new QAction(Lang::get(Lang::PreviousTrack), this);
	_bwd_action->setIcon(GUI::get_icon("bwd"));
	_fwd_action = new QAction(Lang::get(Lang::NextTrack), this);
	_fwd_action->setIcon(GUI::get_icon("fwd"));
	_mute_action = new QAction(Lang::get(Lang::MuteOn), this);
	_mute_action->setIcon(GUI::get_icon("vol_mute"));
	_show_action = new QAction(Lang::get(Lang::Show), this);
	_cur_song_action = new QAction(tr("Current song"), this);
	_cur_song_action->setIcon(GUI::get_icon("info"));
	_close_action = new QAction(Lang::get(Lang::Close), this);
	_close_action->setIcon(GUI::get_icon("power_off"));

	_context_menu = new QMenu();
	_context_menu->addAction(_play_action);
	_context_menu->addAction(_stop_action);
	_context_menu->addSeparator();
	_context_menu->addAction(_fwd_action);
	_context_menu->addAction(_bwd_action);
	_context_menu->addSeparator();
	_context_menu->addAction(_mute_action);
	_context_menu->addSeparator();
	_context_menu->addAction(_cur_song_action);
	_context_menu->addSeparator();
	_context_menu->addAction(_show_action);
	_context_menu->addAction(_close_action);

	this->setContextMenu(_context_menu);

	connect(_play_action, &QAction::triggered, this, &GUI_TrayIcon::play_clicked);
	connect(_fwd_action, &QAction::triggered, this, &GUI_TrayIcon::fwd_clicked);
	connect(_bwd_action, &QAction::triggered, this, &GUI_TrayIcon::bwd_clicked);
	connect(_stop_action, &QAction::triggered, this, &GUI_TrayIcon::stop_clicked);
	connect(_close_action, &QAction::triggered, this, &GUI_TrayIcon::close_clicked);
	connect(_mute_action, &QAction::triggered, this, &GUI_TrayIcon::mute_clicked);
	connect(_cur_song_action, &QAction::triggered, this, &GUI_TrayIcon::cur_song_clicked);
	connect(_show_action, &QAction::triggered, this, &GUI_TrayIcon::show_clicked);

	Set::listen(Set::Player_Style, this, &GUI_TrayIcon::skin_changed);
}

void GUI_TrayIcon::skin_changed()
{
	bool dark = (_settings->get(Set::Player_Style) == 1);

	QString stylesheet = Style::get_style(dark);
	_context_menu->setStyleSheet(stylesheet);

	mute_changed( _settings->get(Set::Engine_Mute) );
}


bool GUI_TrayIcon::event ( QEvent * e )
{
	if (e->type() == QEvent::Wheel) {
		QWheelEvent* wheel_event = static_cast<QWheelEvent*>(e);

		if(wheel_event){
			emit sig_wheel_changed( wheel_event->delta() );
		}
	}

	return true;
}


void GUI_TrayIcon::notify(const MetaData& md)
{
	if ( !isSystemTrayAvailable() ) {
		return;
	}

	QString msg = md.title + " " + Lang::get(Lang::By).space() + md.artist;
	int timeout = _settings->get(Set::Notification_Timeout);

	showMessage("Sayonara", msg, QSystemTrayIcon::Information, timeout);
}


void GUI_TrayIcon::notify(const QString &title, const QString &message, const QString &image_path)
{
	Q_UNUSED(image_path)

	if(!isSystemTrayAvailable()){
		return;
	}

	int timeout = _settings->get(Set::Notification_Timeout);

	showMessage(title, message, QSystemTrayIcon::Information, timeout);
}


void GUI_TrayIcon::playstate_changed(PlayState state)
{
	switch(state)
	{
		case PlayState::Playing:

			setIcon(GUI::get_icon("play"));

			if(_play_action){
				_play_action->setIcon(GUI::get_icon("pause"));
				_play_action->setText(Lang::get(Lang::Pause));
			}

			break;

		default:
			setIcon(GUI::get_icon("pause"));
			if(_play_action){
				_play_action->setIcon(GUI::get_icon("play"));
				_play_action->setText(Lang::get(Lang::Play));
			}

			break;
	}
}


void GUI_TrayIcon::set_enable_fwd(bool b)
{
	if(_fwd_action){
		_fwd_action->setEnabled(b);
	}
}

void GUI_TrayIcon::play_clicked()
{
	_play_manager->play_pause();
}


void GUI_TrayIcon::stop_clicked()
{
	_play_manager->stop();
}


void GUI_TrayIcon::fwd_clicked()
{
	_play_manager->next();
}


void GUI_TrayIcon::bwd_clicked()
{
	_play_manager->previous();
}


void GUI_TrayIcon::show_clicked()
{
	emit sig_show_clicked();
}


void GUI_TrayIcon::close_clicked()
{
	emit sig_close_clicked();
}


void GUI_TrayIcon::mute_clicked()
{
	bool mute = _settings->get(Set::Engine_Mute);

	_play_manager->set_muted(!mute);
}


void GUI_TrayIcon::cur_song_clicked()
{
	NotificationHandler::getInstance()->notify(_play_manager->current_track());
}


void GUI_TrayIcon::mute_changed(bool muted)
{
	QString suffix = "";
	int style = _settings->get(Set::Player_Style);

	if(style == 1) {
		suffix = "_dark";
	}

	if(_mute_action){
		if(!muted) {
			_mute_action->setIcon(GUI::get_icon("vol_mute" + suffix));
			_mute_action->setText(Lang::get(Lang::MuteOn));
		}

		else {
			_mute_action->setIcon(GUI::get_icon("vol_3" + suffix));
			_mute_action->setText(Lang::get(Lang::MuteOff));
		}
	}
}


void GUI_TrayIcon::_sl_show_tray_icon()
{
	bool show_tray_icon = _settings->get(Set::Player_ShowTrayIcon);
	this->setVisible(show_tray_icon);
}
