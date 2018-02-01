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

#include "GUI/Utils/GuiUtils.h"
#include "GUI/Utils/Icons.h"
#include "GUI/Utils/Style.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"

#include "Components/PlayManager/PlayManager.h"

#include <QAction>
#include <QMenu>
#include <QEvent>
#include <QWheelEvent>
#include <QHoverEvent>
#include <QIcon>
#include <QTimer>

using namespace Gui;

struct GUI_TrayIcon::Private
{
	// some shared actions
	PlayManagerPtr play_manager=nullptr;

	QAction*	show_action=nullptr;
	QAction*	close_action=nullptr;
	QAction*	play_action=nullptr;
	QAction*	stop_action=nullptr;
	QAction*	mute_action=nullptr;
	QAction*	fwd_action=nullptr;
	QAction*	bwd_action=nullptr;
	QAction*	cur_song_action=nullptr;

	QMenu*		context_menu=nullptr;
	QTimer*		timer=nullptr;

	Private()
	{
		play_manager = PlayManager::instance();
	}
};

GUI_TrayIcon::GUI_TrayIcon (QObject *parent) :
	QSystemTrayIcon(parent),
	NotificationInterface(),
	SayonaraClass()
{
	m = Pimpl::make<Private>();

	connect(m->play_manager, &PlayManager::sig_playstate_changed, this, &GUI_TrayIcon::playstate_changed);
	connect(m->play_manager, &PlayManager::sig_mute_changed, this, &GUI_TrayIcon::mute_changed);

	init_context_menu();

	playstate_changed(m->play_manager->playstate());

	bool muted = m->play_manager->is_muted();
	mute_changed(muted);

	NotificationHandler::instance()->register_notificator(this);
}

GUI_TrayIcon::~GUI_TrayIcon() {}

void GUI_TrayIcon::init_context_menu()
{
	if(m->context_menu){
		return;
	}

	m->play_action = new QAction(this);
	m->stop_action = new QAction(this);
	m->bwd_action = new QAction(this);
	m->fwd_action = new QAction(this);
	m->mute_action = new QAction(this);
	m->show_action = new QAction(this);
	m->cur_song_action = new QAction(this);
	m->close_action = new QAction(this);

	m->context_menu = new QMenu();
	m->context_menu->addAction(m->play_action);
	m->context_menu->addAction(m->stop_action);
	m->context_menu->addSeparator();
	m->context_menu->addAction(m->fwd_action);
	m->context_menu->addAction(m->bwd_action);
	m->context_menu->addSeparator();
	m->context_menu->addAction(m->mute_action);
	m->context_menu->addSeparator();
	m->context_menu->addAction(m->cur_song_action);
	m->context_menu->addSeparator();
	m->context_menu->addAction(m->show_action);
	m->context_menu->addAction(m->close_action);

	this->setContextMenu(m->context_menu);

	connect(m->play_action, &QAction::triggered, this, &GUI_TrayIcon::play_clicked);
	connect(m->fwd_action, &QAction::triggered, this, &GUI_TrayIcon::fwd_clicked);
	connect(m->bwd_action, &QAction::triggered, this, &GUI_TrayIcon::bwd_clicked);
	connect(m->stop_action, &QAction::triggered, this, &GUI_TrayIcon::stop_clicked);
	connect(m->close_action, &QAction::triggered, this, &GUI_TrayIcon::close_clicked);
	connect(m->mute_action, &QAction::triggered, this, &GUI_TrayIcon::mute_clicked);
	connect(m->cur_song_action, &QAction::triggered, this, &GUI_TrayIcon::cur_song_clicked);
	connect(m->show_action, &QAction::triggered, this, &GUI_TrayIcon::show_clicked);

	Set::listen<Set::Player_Language>(this, &GUI_TrayIcon::language_changed);
	Set::listen<Set::Player_Style>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Player_FontName>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Player_FontSize>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Lib_FontSize>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Lib_FontBold>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Icon_Theme>(this, &GUI_TrayIcon::skin_changed);
	Set::listen<Set::Icon_ForceInDarkTheme>(this, &GUI_TrayIcon::skin_changed);
}


void GUI_TrayIcon::language_changed()
{
	m->play_action->setText(Lang::get(Lang::PlayPause));
	m->fwd_action->setText(Lang::get(Lang::NextTrack));
	m->bwd_action->setText(Lang::get(Lang::PreviousTrack));
	m->stop_action->setText(Lang::get(Lang::Stop));

	if(m->play_manager->is_muted()){
		m->mute_action->setText(Lang::get(Lang::MuteOff));
	}

	else {
		m->mute_action->setText(Lang::get(Lang::MuteOn));
	}

	m->close_action->setText(Lang::get(Lang::Quit));
	m->show_action->setText(Lang::get(Lang::Show));
	m->cur_song_action->setText(tr("Current song"));
}


void GUI_TrayIcon::skin_changed()
{
	bool dark = (_settings->get<Set::Player_Style>() == 1);

	QString stylesheet = Style::style(dark);
	m->context_menu->setStyleSheet(stylesheet);

	mute_changed( _settings->get<Set::Engine_Mute>() );

	using namespace Gui;
	m->play_action->setIcon(Icons::icon(Icons::Play));
	m->stop_action->setIcon(Icons::icon(Icons::Stop));
	m->bwd_action->setIcon(Icons::icon(Icons::Previous));
	m->fwd_action->setIcon(Icons::icon(Icons::Next));
	m->cur_song_action->setIcon(Icons::icon(Icons::Info));
	m->close_action->setIcon(Icons::icon(Icons::Exit));

	if(m->play_manager->is_muted()){
		m->mute_action->setIcon(Icons::icon(Icons::Vol3));
	}

	else {
		m->mute_action->setIcon(Icons::icon(Icons::VolMute));
	}
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

	QString msg = md.title() + " " + Lang::get(Lang::By).space() + md.artist();
	int timeout = _settings->get<Set::Notification_Timeout>();

	showMessage("Sayonara", msg, QSystemTrayIcon::Information, timeout);
}


void GUI_TrayIcon::notify(const QString &title, const QString &message, const QString &image_path)
{
	Q_UNUSED(image_path)

	if(!isSystemTrayAvailable()){
		return;
	}

	int timeout = _settings->get<Set::Notification_Timeout>();

	showMessage(title, message, QSystemTrayIcon::Information, timeout);
}


// dbus
QString GUI_TrayIcon::name() const
{
	return "Standard";
}

QString GUI_TrayIcon::display_name() const
{
	return Lang::get(Lang::Default);
}

void GUI_TrayIcon::playstate_changed(PlayState state)
{
	using namespace Gui;
	switch(state)
	{
		case PlayState::Playing:

			setIcon(Icons::icon(Icons::Play, Icons::ForceSayonaraIcon));

			if(m->play_action){
				m->play_action->setIcon(Icons::icon(Icons::Pause));
				m->play_action->setText(Lang::get(Lang::Pause));
			}

			break;

		default:
			setIcon(Icons::icon(Icons::Pause, Icons::ForceSayonaraIcon));

			if(m->play_action){
				m->play_action->setIcon(Icons::icon(Icons::Play));
				m->play_action->setText(Lang::get(Lang::Play));
			}

			break;
	}
}


void GUI_TrayIcon::set_enable_fwd(bool b)
{
	if(m->fwd_action){
		m->fwd_action->setEnabled(b);
	}
}

void GUI_TrayIcon::play_clicked()
{
	m->play_manager->play_pause();
}


void GUI_TrayIcon::stop_clicked()
{
	m->play_manager->stop();
}


void GUI_TrayIcon::fwd_clicked()
{
	m->play_manager->next();
}


void GUI_TrayIcon::bwd_clicked()
{
	m->play_manager->previous();
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
	bool mute = _settings->get<Set::Engine_Mute>();

	m->play_manager->set_muted(!mute);
}


void GUI_TrayIcon::cur_song_clicked()
{
	NotificationHandler::instance()->notify(m->play_manager->current_track());
}


void GUI_TrayIcon::mute_changed(bool muted)
{
	using namespace Gui;
	if(m->mute_action){
		if(!muted) {
			m->mute_action->setIcon(Icons::icon(Icons::VolMute));
			m->mute_action->setText(Lang::get(Lang::MuteOn));
		}

		else {
			m->mute_action->setIcon(Icons::icon(Icons::Vol3));
			m->mute_action->setText(Lang::get(Lang::MuteOff));
		}
	}
}


void GUI_TrayIcon::_sl_show_tray_icon()
{
	bool show_tray_icon = _settings->get<Set::Player_ShowTrayIcon>();
	this->setVisible(show_tray_icon);
}
