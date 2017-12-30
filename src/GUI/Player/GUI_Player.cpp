/* GUI_Player.cpp */

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

#include "GUI_Player.h"
#include "GUI_Logger.h"
#include "GUI_TrayIcon.h"
#include "GUI_PlayerMenubar.h"

#include "Components/PlayManager/PlayManager.h"

#include "Utils/Message/Message.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Language.h"
#include "Utils/Utils.h"
#include "Utils/globals.h"
#include "Utils/WebAccess/AsyncWebAccess.h"
#include "Utils/Settings/Settings.h"
#include "Utils/MetaData/MetaData.h"

#include "GUI/Utils/Icons.h"
#include "GUI/Utils/GuiUtils.h"
#include "GUI/Utils/Style.h"

#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Interfaces/PreferenceDialog/PreferenceDialog.h"

#include <QTranslator>
#include <QAction>
#include <QHBoxLayout>
#include <QShowEvent>
#include <QCloseEvent>

struct GUI_Player::Private
{
	PlayerPlugin::Handler*		pph=nullptr;
	Menubar*					menubar=nullptr;
	QTranslator*				translator=nullptr;
	GUI_Logger*					logger=nullptr;
	QPoint						initial_pos;
	QSize						initial_sz;

	Private(QTranslator* translator) :
		translator(translator)
	{
		Settings* s = Settings::instance();
		logger = new GUI_Logger();

		initial_pos = s->get(Set::Player_Pos);
		initial_sz = s->get(Set::Player_Size);
	}

	~Private()
	{
		delete logger; logger = nullptr;
	}
};


GUI_Player::GUI_Player(QTranslator* translator, QWidget* parent) :
	Gui::MainWindow(parent),
	GlobalMessageReceiverInterface("Player Main Window"),
	Ui::Sayonara()
{
	m = Pimpl::make<Private>(translator);
	setupUi(this);

	GlobalMessage::register_receiver(this);

	m->menubar = new Menubar(this);
	this->setMenuBar(m->menubar);

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	lph->set_library_parent(this->library_widget);

	QString version = _settings->get(Set::Player_Version);
	setWindowTitle(QString("Sayonara %1").arg(version));
	setWindowIcon(Gui::Util::icon("logo.png"));
	setAttribute(Qt::WA_DeleteOnClose, false);

	init_tray_actions();
	init_connections();
	init_sizes();
	init_splitter();

	if(_settings->get(Set::Player_NotifyNewVersion))
	{
		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->run("http://sayonara-player.com/current_version");
		connect(awa, &AsyncWebAccess::sig_finished, this, &GUI_Player::awa_version_finished);
	}

	Set::listen(Set::Player_Fullscreen, this, &GUI_Player::fullscreen_changed, false);
	Set::listen(SetNoDB::Player_Quit, this, &GUI_Player::really_close, false);
	Set::listen(Set::Lib_Show, this, &GUI_Player::show_library_changed, false);
}

GUI_Player::~GUI_Player()
{
	sp_log(Log::Debug, this) << "Player closed.";
}

void GUI_Player::init_sizes()
{
	if(_settings->get(Set::Player_StartInTray)){
		this->setHidden(true);
	}

	else if(_settings->get(Set::Player_Fullscreen))
	{
		this->showFullScreen();
	}

	else if(_settings->get(Set::Player_Maximized))
	{
		this->showMaximized();
	}

	else
	{
		QPoint pos = m->initial_pos;
		QSize sz = m->initial_sz;

		this->showNormal();
		this->setGeometry(pos.x(), pos.y(), sz.width(), sz.height());
	}
}

void GUI_Player::init_splitter()
{
	if(_settings->get(Set::Lib_Show)){
		splitter->widget(1)->show();
	}
	else{
		splitter->widget(1)->hide();
	}

	QByteArray splitter_state = _settings->get(Set::Player_SplitterState);
	if(splitter_state.size() <= 1)
	{
		int w1 = width() / 3;
		int w2 = width() - w1;
		splitter->setSizes({w1, w2});

		_settings->set(Set::Player_SplitterState, splitter->saveState());
	}

	else {
		splitter->restoreState(splitter_state);
	}
}


void GUI_Player::init_connections()
{
	PlayManagerPtr play_manager = PlayManager::instance();
	Library::PluginHandler* lph = Library::PluginHandler::instance();

	connect(lph, &Library::PluginHandler::sig_current_library_changed,
			this, &GUI_Player::current_library_changed);

	connect(lph, &Library::PluginHandler::sig_initialized,
			this, &GUI_Player::check_library_menu_action);

	connect(lph, &Library::PluginHandler::sig_libraries_changed,
			this, &GUI_Player::check_library_menu_action);

	connect(play_manager, &PlayManager::sig_playstate_changed, this, &GUI_Player::playstate_changed);
	connect(play_manager, &PlayManager::sig_error, this, &GUI_Player::play_error);

	connect(splitter, &QSplitter::splitterMoved, this, &GUI_Player::main_splitter_moved);

	connect(m->menubar, &Menubar::sig_close_clicked, this, &GUI_Player::really_close);
	connect(m->menubar, &Menubar::sig_logger_clicked, m->logger, &GUI_Logger::show);
}


/** TRAY ICON **/
void GUI_Player::init_tray_actions()
{
	GUI_TrayIcon* tray_icon = new GUI_TrayIcon(this);
	tray_icon->installEventFilter(this);

	connect(tray_icon, &GUI_TrayIcon::sig_close_clicked, this, &GUI_Player::really_close);
	connect(tray_icon, &GUI_TrayIcon::sig_show_clicked, this, &GUI_Player::raise);
	connect(tray_icon, &GUI_TrayIcon::sig_wheel_changed, controls, &GUI_Controls::change_volume_by_tick);
	connect(tray_icon, &GUI_TrayIcon::activated, this, &GUI_Player::tray_icon_activated);

	if(_settings->get(Set::Player_ShowTrayIcon)){
		tray_icon->show();
	}
}


void GUI_Player::tray_icon_activated(QSystemTrayIcon::ActivationReason reason)
{
	bool min_to_tray = _settings->get(Set::Player_Min2Tray);
	switch (reason)
	{
		case QSystemTrayIcon::Trigger:
			if( this->isMinimized() ||
				!this->isVisible() ||
				!this->isActiveWindow())
			{
				raise();
			}

			else
			{
				if(min_to_tray) {
					this->setHidden(true);
				}

				else{
					this->showMinimized();
				}
			}

			break;

		default:
			break;
	}
}


void GUI_Player::current_library_changed(const QString& name)
{
	Q_UNUSED(name)

	check_library_menu_action();
}

void GUI_Player::check_library_menu_action()
{
	QList<Library::Container*> libraries;

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	libraries = lph->get_libraries();
	if(!lph->current_library()){
		m->menubar->show_library_action(false);
	}

	else {
		m->menubar->update_library_action(lph->current_library_menu(), lph->current_library()->display_name());
	}
}


void GUI_Player::register_preference_dialog(PreferenceDialog* dialog)
{
	m->menubar->insert_preference_action(dialog->action());
}


void GUI_Player::playstate_changed(PlayState state)
{
	switch(state)
	{
		case PlayState::Stopped:
			setWindowTitle("Sayonara Player");
			break;
		default:
			break;
	}
}


void GUI_Player::play_error(const QString& message)
{
	const MetaData& md = PlayManager::instance()->current_track();
	QString err = message + "\n\n" + md.filepath();
	Message::warning(err, Lang::get(Lang::Play));
}


void GUI_Player::register_player_plugin_handler(PlayerPlugin::Handler* pph)
{
	m->pph = pph;
	if(!pph){
		return;
	}

	QList<PlayerPlugin::Base*> lst = m->pph->get_all_plugins();

	int i=1;
	for(PlayerPlugin::Base* p : lst)
	{
		QAction* action = p->get_action();
		QKeySequence ks("Ctrl+F" + QString::number(i));
		action->setShortcut(ks);
		action->setData(p->get_name());
		m->menubar->insert_player_plugin_action(action);

		i++;
	}

	connect(m->pph, &PlayerPlugin::Handler::sig_plugin_closed, this, &GUI_Player::plugin_closed);
	connect(m->pph, &PlayerPlugin::Handler::sig_plugin_action_triggered, this, &GUI_Player::plugin_action_triggered);

	plugin_opened();
}


void GUI_Player::plugin_action_triggered(bool b)
{
	if(m->pph && b)
	{
		plugin_opened();
	}

	else{
		plugin_closed();
	}
}

void GUI_Player::plugin_opened()
{
	PlayerPlugin::Base* current_plugin=nullptr;

	if(m->pph)
	{
		current_plugin = m->pph->current_plugin();
	}

	if(current_plugin)
	{
		plugin_widget->show(current_plugin);
	}

	else {
		plugin_widget->hide();
	}
}

void GUI_Player::plugin_closed()
{
	plugin_widget->close();
}


void GUI_Player::awa_version_finished()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!awa){
		return;
	}

	if(awa->status() != AsyncWebAccess::Status::GotData) {
		awa->deleteLater();
		return;
	}

	QString new_version(awa->data());
	QString cur_version = _settings->get(Set::Player_Version);
	bool notify_new_version = _settings->get(Set::Player_NotifyNewVersion);
	bool dark = (_settings->get(Set::Player_Style) == 1);

	new_version = new_version.trimmed();

	sp_log(Log::Info, this) << "Newest Version: " << new_version;
	sp_log(Log::Info, this) << "This Version:   " << cur_version;

	QString link;
	LINK("http://sayonara-player.com", "http://sayonara-player.com", dark, link);

	if(new_version > cur_version && notify_new_version) {
		Message::info(tr("A new version is available!") + "<br />" +  link);
	}

	awa->deleteLater();
}


void GUI_Player::language_changed()
{
	QString language = _settings->get(Set::Player_Language);
	m->translator->load(language, Util::share_path("translations/"));

	retranslateUi(this);
}

void GUI_Player::fullscreen_changed()
{
	bool b = _settings->get(Set::Player_Fullscreen);

	if(b){
		showFullScreen();
	}

	else {
		showNormal();
	}
}

void GUI_Player::show_library_changed()
{
	bool b = _settings->get(Set::Lib_Show);

	QSize player_size = this->size();

	int library_width = library_widget->width();

	// we need to add use hide/show on splitter
	if(!b)
	{
		splitter->widget(1)->hide();
		if(_settings->get(Set::Lib_OldWidth) == 0){
			_settings->set(Set::Lib_OldWidth, library_width);
		}

		player_size.setWidth( player_size.width() - library_width);
	}

	else
	{
		splitter->widget(1)->show();
		library_width = _settings->get(Set::Lib_OldWidth);
		_settings->set(Set::Lib_OldWidth, 0);

		if(library_width < 100){
			library_width = 400;
		}

		player_size.setWidth( player_size.width() + library_width);
	}

	check_library_menu_action();

	if(!this->isMaximized() && !this->isFullScreen()){
		this->resize(player_size);
	}
}


void GUI_Player::skin_changed()
{
	bool dark = (_settings->get(Set::Player_Style) == 1);

	QString stylesheet = Style::style(dark);
	this->setStyleSheet(stylesheet);
}


void GUI_Player::really_close()
{
	sp_log(Log::Info, this) << "closing player...";

	QMainWindow::close();

	emit sig_player_closed();
}

void GUI_Player::moveEvent(QMoveEvent *e)
{
	QMainWindow::moveEvent(e);

	QPoint p= this->pos();
	_settings->set(Set::Player_Pos, p);
}


void GUI_Player::resizeEvent(QResizeEvent* e)
{
	QMainWindow::resizeEvent(e);
	Library::PluginHandler* lph = Library::PluginHandler::instance();

	bool is_maximized = _settings->get(Set::Player_Maximized);
	bool is_fullscreen = _settings->get(Set::Player_Fullscreen);
	bool is_library_visible = _settings->get(Set::Lib_Show);

	if(is_maximized) {
		_settings->set(Set::Player_Fullscreen, false);
	}

	if(is_library_visible && lph){
		Library::Container* container;
		container = lph->current_library();
		if(container && container->is_initialized()){
			container->widget()->resize(library_widget->size());
		}
	}

	if( !is_maximized &&
		!this->isMaximized() &&
		!is_fullscreen &&
		!this->isFullScreen())
	{
		_settings->set(Set::Player_Size, this->size());
	}

	update();
}


void GUI_Player::main_splitter_moved(int pos, int idx)
{
	Q_UNUSED(pos)
	Q_UNUSED(idx)

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	if(lph)
	{
		Library::Container* container;
		container = lph->current_library();
		if(container){
			container->widget()->resize(library_widget->size());
		}
	}

	QByteArray splitter_state = splitter->saveState();
	_settings->set(Set::Player_SplitterState, splitter_state);
}


void GUI_Player::closeEvent(QCloseEvent* e)
{
	e->ignore();

	bool min_to_tray = _settings->get(Set::Player_Min2Tray);

	if(min_to_tray && !this->isHidden()) {
		this->hide();
	}

	_settings->set(Set::Player_Maximized, this->isMaximized());
	_settings->set(Set::Player_Fullscreen, this->isFullScreen());
	_settings->set(Set::Player_Pos, this->pos());

	if(!min_to_tray){
		really_close();
	}
}
