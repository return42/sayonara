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

GUI_Player::GUI_Player(QTranslator* translator, QWidget* parent) :
	Gui::MainWindow(parent),
	GlobalMessageReceiverInterface("Player Main Window"),
	Ui::Sayonara()
{
	setupUi(this);

	_translator = translator;
	_logger = new GUI_Logger();

	GlobalMessage::register_receiver(this);

	init_gui();

	setup_tray_actions();
	setup_connections();

	Set::listen(SetNoDB::Player_Quit, this, &GUI_Player::really_close, false);

	Set::listen(Set::Player_FontName, this, &GUI_Player::skin_changed);
	Set::listen(Set::Player_FontSize, this, &GUI_Player::skin_changed);
	Set::listen(Set::PL_FontSize, this, &GUI_Player::skin_changed);
	Set::listen(Set::Lib_FontSize, this, &GUI_Player::skin_changed);
	Set::listen(Set::Lib_FontBold, this, &GUI_Player::skin_changed);
}


GUI_Player::~GUI_Player()
{
	sp_log(Log::Debug, this) << "Player closed.";
	delete _logger; _logger=nullptr;
}


void GUI_Player::init_gui()
{
	Library::PluginHandler::instance()->set_library_parent(this->library_widget);
	action_viewLibrary->setChecked(_settings->get(Set::Lib_Show));
	action_viewLibrary->setText(Lang::get(Lang::Library));

	action_Dark->setChecked(_settings->get(Set::Player_Style));
	action_Dark->setShortcut(QKeySequence("F10"));

	action_Fullscreen->setShortcut(QKeySequence("F11"));

#ifdef WITH_SHUTDOWN
	QList<QAction*> actions = menu_file->actions();
	QAction* sep = actions[actions.size() - 1];
	_action_shutdown = new QAction(Gui::Icons::icon(Gui::Icons::Shutdown), Lang::get(Lang::Shutdown).triplePt(), this);
	connect(_action_shutdown, &QAction::triggered, this, &GUI_Player::shutdown_clicked);

	menu_file->insertAction(sep, _action_shutdown);
#endif

	bool library_visible = _settings->get(Set::Lib_Show);
	show_library(library_visible);

	QString version = _settings->get(Set::Player_Version);
	setWindowTitle(QString("Sayonara %1").arg(version));
	setWindowIcon(Gui::Util::icon("logo.png"));
	setAttribute(Qt::WA_DeleteOnClose, false);

	plugin_widget->hide();

	language_changed();
}


void GUI_Player::ui_loaded()
{
	skin_changed();

	bool fullscreen = _settings->get(Set::Player_Fullscreen);
	bool maximized = _settings->get(Set::Player_Maximized);
	QPoint pos = _settings->get(Set::Player_Pos);
	QSize sz = _settings->get(Set::Player_Size);

	action_Fullscreen->setChecked(fullscreen);

	this->setGeometry(pos.x(), pos.y(), sz.width(), sz.height());

	if(fullscreen){
		this->showFullScreen();
	}

	else if(maximized){
		this->showMaximized();
	}

	else{
		this->showNormal();
	}

	if(_settings->get(Set::Player_NotifyNewVersion)){
		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->run("http://sayonara-player.com/current_version");
		connect(awa, &AsyncWebAccess::sig_finished, this, &GUI_Player::awa_version_finished);
	}

	QLayout* layout;
	splitter->restoreState(_settings->get(Set::Player_SplitterState));
	layout = splitter->layout();
	if(layout){
		layout->update();
	}

	splitter->update();

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	Library::Container* current_library = lph->current_library();
	if(current_library)
	{
		QWidget* current_library_widget = current_library->widget();

		if(current_library_widget == nullptr) return;

		current_library_widget->show();
		current_library_widget->resize(library_widget->size());
	}

	if(_settings->get(Set::Player_StartInTray)){
		this->setHidden(true);
	}

	plugin_opened();
}


void GUI_Player::setup_connections()
{
	PlayManagerPtr play_manager = PlayManager::instance();
	Library::PluginHandler* lph = Library::PluginHandler::instance();

	connect(lph, &Library::PluginHandler::sig_current_library_changed,
			this, &GUI_Player::current_library_changed);

	connect(lph, &Library::PluginHandler::sig_initialized,
			this, &GUI_Player::check_library_menu_action);

	connect(play_manager, &PlayManager::sig_playstate_changed, this, &GUI_Player::playstate_changed);
	connect(play_manager, &PlayManager::sig_error, this, &GUI_Player::play_error);

	// file
	connect(action_OpenFile, &QAction::triggered, this, &GUI_Player::open_files_clicked);
	connect(action_OpenFolder, &QAction::triggered, this, &GUI_Player::open_dir_clicked);
	connect(action_Close, &QAction::triggered, this, &GUI_Player::really_close);


	// view
	connect(action_viewLibrary, &QAction::toggled, this, &GUI_Player::show_library);
	connect(action_Dark, &QAction::toggled, this, &GUI_Player::skin_toggled);
	connect(action_Fullscreen, &QAction::toggled, this, &GUI_Player::show_fullscreen_toggled);
	connect(action_logger, &QAction::triggered, _logger, &GUI_Logger::show);

	connect(splitter, &QSplitter::splitterMoved, this, &GUI_Player::main_splitter_moved);

	// about
	connect(action_about, &QAction::triggered, this, &GUI_Player::about);
	connect(action_help, &QAction::triggered, this, &GUI_Player::help);
}


/** TRAY ICON **/
void GUI_Player::setup_tray_actions()
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


void GUI_Player::tray_icon_activated (QSystemTrayIcon::ActivationReason reason)
{
	bool min_to_tray = _settings->get(Set::Player_Min2Tray);
	switch (reason) {
	case QSystemTrayIcon::Trigger:

		if( this->isMinimized() ||
			!this->isVisible() ||
			!this->isActiveWindow())

		{
			raise();
		}

		else{
			if(min_to_tray){
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
	bool library_visible;

	Library::PluginHandler* lph = Library::PluginHandler::instance();
	libraries = lph->get_libraries();
	if(libraries.isEmpty()){
		return;
	}

	library_visible = _settings->get(Set::Lib_Show);

	for(Library::Container* container : libraries)
	{
		Library::Container* library = lph->current_library();
		QMenu* menu = lph->current_library_menu();
		if(!menu){
			continue;
		}

		QAction* action;
		if( library == container && library_visible)
		{
			action = menubar->insertMenu(menu_about->menuAction(), menu);

			if(action){
				action->setText(library->display_name());
			}

			library->set_menu_action(action);
		}

		else
		{
			action = container->menu_action();
			if(action){
				sp_log(Log::Debug, this) << "Remove menu for " << container->display_name();
				menubar->removeAction(action);
			}
		}
	}
}


/** LIBRARY AND PLAYLIST END **/
void GUI_Player::register_preference_dialog(PreferenceDialog* dialog)
{
	QList<QAction*> actions = menu_file->actions();
	QAction* sep = actions[actions.size() - 3];

	QAction* preference_action = dialog->action();

	menu_file->insertAction(sep, preference_action);
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
	QList<PlayerPlugin::Base*> lst = pph->get_all_plugins();
	QList<QAction*> actions;

	int i=1;
	for(PlayerPlugin::Base* p : lst) {
		QAction* action = p->get_action();
		QKeySequence ks("Ctrl+F" + QString::number(i));
		action->setShortcut(ks);
		action->setData(p->get_name());
		actions << action;

		i++;
	}

	connect(pph, &PlayerPlugin::Handler::sig_plugin_closed, this, &GUI_Player::plugin_closed);
	connect(pph, &PlayerPlugin::Handler::sig_plugin_action_triggered, this, &GUI_Player::plugin_action_triggered);

	menu_view->insertActions(action_Dark, actions);
	menu_view->insertSeparator(action_Dark);
}

void GUI_Player::plugin_action_triggered(bool b)
{
	if(_pph && b)
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

	if(_pph)
	{
		current_plugin = _pph->current_plugin();
	}

	if(current_plugin)
	{
		plugin_widget->show(current_plugin);
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


void GUI_Player::awa_translators_finished()
{
	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!awa){
		return;
	}

	if(awa->status() != AsyncWebAccess::Status::GotData) {
		awa->deleteLater();
		return;
	}

	QString data(awa->data());
	QStringList translators = data.split('\n');

	_translators.clear();

	for(const QString& str : translators) {
		if(str.trimmed().size() > 0) {
			_translators.push_back(str);
		}
	}

	awa->deleteLater();
	about();
}


void GUI_Player::language_changed()
{
	QString language = _settings->get(Set::Player_Language);
	_translator->load(language, Util::share_path("translations/"));

	retranslateUi(this);

	menu_file->setTitle(Lang::get(Lang::File));
	action_OpenFile->setText(Lang::get(Lang::OpenFile).triplePt());
	action_OpenFolder->setText(Lang::get(Lang::OpenDir).triplePt());
	action_Close->setText(Lang::get(Lang::Close));
	action_viewLibrary->setText(Lang::get(Lang::Library));
	action_logger->setText(Lang::get(Lang::Logger));
	action_about->setText(Lang::get(Lang::About).triplePt());

	if(_action_shutdown){
		_action_shutdown->setText(Lang::get(Lang::Shutdown));
	}
}


void GUI_Player::skin_changed()
{
	bool dark = (_settings->get(Set::Player_Style) == 1);

	QString stylesheet = Style::style(dark);
	this->setStyleSheet(stylesheet);

	using namespace Gui;
	action_OpenFile->setIcon(Icons::icon(Icons::Open));
	action_OpenFolder->setIcon(Icons::icon(Icons::Open));
	action_Close->setIcon(Icons::icon(Icons::Exit));
}

void GUI_Player::skin_toggled(bool on)
{
	_settings->set(Set::Player_Style, (on ? 1 : 0) );
}


void GUI_Player::really_close()
{
	sp_log(Log::Info, this) << "closing player...";

	QMainWindow::close();

	emit sig_player_closed();
}
