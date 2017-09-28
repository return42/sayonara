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
#include "GUI/Playlist/GUI_Playlist.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/Style/Style.h"

#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Message/Message.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Helper/Helper.h"
#include "Helper/globals.h"
#include "Helper/WebAccess/AsyncWebAccess.h"
#include "Helper/Settings/Settings.h"

#include "Components/PlayManager/PlayManager.h"

#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"

#include <QDateTime>
#include <QTranslator>

GUI_Player::GUI_Player(QTranslator* translator, QWidget* parent) :
	SayonaraMainWindow(parent),
	ShortcutWidget(),
	GlobalMessageReceiverInterface("Player Main Window"),
	Ui::Sayonara()
{
	setupUi(this);

	_translator = translator;
	_play_manager = PlayManager::getInstance();
	_logger = new GUI_Logger();

	GlobalMessage::getInstance()->register_receiver(this);

	init_gui();

	setup_tray_actions();
	setup_connections();

	REGISTER_LISTENER(Set::Engine_SR_Active, _sl_sr_active_changed);
	REGISTER_LISTENER_NO_CALL(SetNoDB::Player_Quit, really_close);

	REGISTER_LISTENER(Set::Player_FontName, skin_changed);
	REGISTER_LISTENER(Set::Player_FontSize, skin_changed);
	REGISTER_LISTENER(Set::PL_FontSize, skin_changed);
	REGISTER_LISTENER(Set::Lib_FontSize, skin_changed);
	REGISTER_LISTENER(Set::Lib_FontBold, skin_changed);
	REGISTER_LISTENER(Set::Engine_Pitch, file_info_changed);
	REGISTER_LISTENER(Set::Engine_SpeedActive, file_info_changed);
}


GUI_Player::~GUI_Player()
{
	sp_log(Log::Debug, this) << "Player closed.";
	delete _logger; _logger=nullptr;
}


void GUI_Player::init_gui()
{
	QString version = _settings->get(Set::Player_Version);

	progress_widget->setCurrentIndex(0);

	lab_sayonara->setText(tr("Sayonara Player"));
	lab_version->setText( version );
	lab_writtenby->setText(tr("Written by") + " Lucio Carreras");
	lab_copyright->setText(tr("Copyright") + " 2011-" + QString::number(QDateTime::currentDateTime().date().year()));

	LibraryPluginHandler::getInstance()->set_library_parent(this->library_widget);
	action_viewLibrary->setChecked(_settings->get(Set::Lib_Show));
	action_viewLibrary->setText(Lang::get(Lang::Library));

	action_Dark->setChecked(_settings->get(Set::Player_Style));
	action_Dark->setShortcut(QKeySequence("F10"));

	action_Fullscreen->setShortcut(QKeySequence("F11"));

#ifdef WITH_MTP
	action_devices->setVisible(true);
#else
	action_devices->setVisible(false);
#endif

	btn_rec->setVisible(false);

	cur_pos_changed(_play_manager->get_init_position_ms());

	bool library_visible = _settings->get(Set::Lib_Show);
	show_library(library_visible);

	int volume = _play_manager->get_volume();
	volume_changed(volume);

	bool muted = _play_manager->get_mute();
	mute_changed(muted);

	setWindowTitle(QString("Sayonara %1").arg(version));
	setWindowIcon(GUI::get_icon("logo.png"));
	setAttribute(Qt::WA_DeleteOnClose, false);

	plugin_widget->resize(plugin_widget->width(), 0);
	plugin_widget->hide();

	language_changed();
}

void GUI_Player::language_changed()
{
	QString language = _settings->get(Set::Player_Language);
	_translator->load(language, Helper::share_path("translations/"));

	retranslateUi(this);

	menu_file->setTitle(Lang::get(Lang::File));
	action_OpenFile->setText(Lang::get(Lang::OpenFile).triplePt());
	action_OpenFolder->setText(Lang::get(Lang::OpenDir).triplePt());
	action_Close->setText(Lang::get(Lang::Close));
	action_viewLibrary->setText(Lang::get(Lang::Library));
	action_logger->setText(Lang::get(Lang::Logger));
	action_about->setText(Lang::get(Lang::About).triplePt());
}


// new track
void GUI_Player::track_changed(const MetaData & md)
{
	lab_sayonara->hide();
	lab_title->show();

	lab_version->hide();
	lab_artist->show();

	lab_writtenby->hide();
	lab_album->show();

	lab_copyright->hide();
	lab_rating->show();

	set_info_labels(md);
	set_cur_pos_label(0);
	set_total_time_label(md.length_ms);
	file_info_changed();
	set_cover_location(md);
	set_radio_mode( md.radio_mode() );

	sli_progress->setEnabled( (md.length_ms / 1000) > 0 );

	this->setWindowTitle(QString("Sayonara - ") + md.title);
	this->repaint();
}


void GUI_Player::refresh_info_labels()
{
	set_info_labels(_play_manager->get_cur_track());
}

void GUI_Player::set_info_labels(const MetaData& md)
{
	set_title_label(md.title);
	set_album_label(md.album, md.year);
	set_artist_label(md.artist);
}


void GUI_Player::set_title_label(const QString& title)
{
	QString text = GUI::elide_text(title, lab_title, 2);
	lab_title->setText(text);
}


void GUI_Player::set_album_label(const QString& album, int year)
{
	QString str_year = QString::number(year);
	QString album_name(album);
	QFontMetrics fm(lab_album->fontMetrics());

	if(year > 1000 && !album_name.contains(str_year)){
		album_name += " (" + str_year + ")";
	}

	lab_album->setText( fm.elidedText(album_name, Qt::ElideRight, lab_album->width()) );
}


void GUI_Player::set_artist_label(const QString& artist)
{
	QFontMetrics fm = lab_artist->fontMetrics();
	lab_artist->setText( fm.elidedText(artist, Qt::ElideRight, lab_artist->width()) );
}


void GUI_Player::dur_changed(const MetaData& md)
{
	set_total_time_label(md.length_ms);
}

void GUI_Player::br_changed(const MetaData& md)
{
	QString rating_text;

	if(md.bitrate / 1000 > 0){
		rating_text = QString::number(md.bitrate / 1000) + " kBit/s";
	}

	if(md.filesize > 0){
		if(md.bitrate / 1000 > 0){
			rating_text += ", ";
		}
		rating_text += QString::number( (double) (md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);
}


void GUI_Player::md_changed(const MetaData& md)
{
	md.print();
	MetaData modified_md(md);

	if(md.radio_mode() == RadioMode::Station){
		modified_md.album = md.album + " (" + md.filepath() + ")";
	}

	set_info_labels(modified_md);
}


// public slot:
// id3 tags have changed
void GUI_Player::id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	Q_UNUSED(v_md_old)
	Q_UNUSED(v_md_new)

	MetaData md = _play_manager->get_cur_track();

	set_info_labels(md);
	set_cover_location(md);

	setWindowTitle(QString("Sayonara - ") + md.title);
}

void GUI_Player::skin_changed()
{
	IconLoader* icon_loader = IconLoader::getInstance();
	bool dark = (_settings->get(Set::Player_Style) == 1);

	QString stylesheet = Style::get_style(dark);

	this->setStyleSheet(stylesheet);

	btn_fw->setIcon(icon_loader->get_icon("media-skip-forward", "fwd"));
	btn_bw->setIcon(icon_loader->get_icon("media-skip-backward", "bwd"));

	if(_play_manager->get_play_state() == PlayState::Playing){
		btn_play->setIcon(icon_loader->get_icon("media-playback-pause", "pause"));
	}

	else{
		btn_play->setIcon(icon_loader->get_icon("media-playback-start", "play"));
	}

	btn_stop->setIcon(icon_loader->get_icon("media-playback-stop", "stop"));
	btn_rec->setIcon(icon_loader->get_icon("media-record", "rec"));
	action_OpenFile->setIcon(icon_loader->get_icon("document-open", "play"));
	action_OpenFolder->setIcon(icon_loader->get_icon("document-open", "play"));
	action_Close->setIcon(icon_loader->get_icon("window-close", "power_off"));

	setup_volume_button(sli_volume->value());
}

void GUI_Player::skin_toggled(bool on)
{
	_settings->set(Set::Player_Style, (on ? 1 : 0) );
}


/** TRAY ICON **/
void GUI_Player::setup_tray_actions()
{
	GUI_TrayIcon* tray_icon = new GUI_TrayIcon(this);
	tray_icon->installEventFilter(this);

	connect(tray_icon, &GUI_TrayIcon::sig_close_clicked, this, &GUI_Player::really_close);
	connect(tray_icon, &GUI_TrayIcon::sig_show_clicked, this, &GUI_Player::raise);
	connect(tray_icon, &GUI_TrayIcon::sig_wheel_changed, this, &GUI_Player::change_volume_by_tick);
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
	QList<LibraryContainerInterface*> libraries;
	bool library_visible;

	LibraryPluginHandler* lph = LibraryPluginHandler::getInstance();
	libraries = lph->get_libraries();
	if(libraries.isEmpty()){
		return;
	}

	library_visible = _settings->get(Set::Lib_Show);

	for(LibraryContainerInterface* container : libraries)
	{
		QAction* action;
		LibraryContainerInterface* library = lph->current_library();
		QMenu* menu = lph->current_library_menu();
		if(!menu){
			continue;
		}

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

void GUI_Player::register_player_plugin_handler(PlayerPluginHandler* pph)
{
	_pph = pph;

	QList<PlayerPluginInterface*> lst = pph->get_all_plugins();
	QList<QAction*> actions;

	int i=1;
	for(PlayerPluginInterface* p : lst) {
		QAction* action = p->get_action();
		QKeySequence ks("Ctrl+F" + QString::number(i));
		action->setShortcut(ks);
		action->setData(p->get_name());
		actions << action;

		i++;
	}

	menu_view->insertActions(action_Dark, actions);
	menu_view->insertSeparator(action_Dark);
}


/** LIBRARY AND PLAYLIST END **/
void GUI_Player::register_preference_dialog(PreferenceDialogInterface* dialog)
{
	QList<QAction*> actions = menu_file->actions();
	QAction* sep = actions[actions.size() - 4];

	dialog->setParent(this);
	menu_file->insertAction(sep, dialog->get_action());
}


void GUI_Player::set_radio_mode(RadioMode radio)
{
	check_record_button_visible();

	if(radio != RadioMode::Off){
		buffering(0);
	}
}

void GUI_Player::_sl_sr_active_changed()
{
	check_record_button_visible();
	btn_rec->setChecked(false);
}

void GUI_Player::check_record_button_visible()
{
	MetaData md = _play_manager->get_cur_track();
	PlayState playstate = _play_manager->get_play_state();

	bool is_lame_available = _settings->get(SetNoDB::MP3enc_found);
	bool is_sr_active = _settings->get(Set::Engine_SR_Active);
	bool is_radio = ((md.radio_mode() != RadioMode::Off));
	bool is_playing = (playstate == PlayState::Playing);

	bool recording_enabled = (is_lame_available &&
							  is_sr_active &&
							  is_radio &&
							  is_playing);

	btn_play->setVisible(!recording_enabled);
	btn_rec->setVisible(recording_enabled);

	if(!recording_enabled){
		btn_rec->setChecked(false);
	}
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


	if(_play_manager->get_play_state() != PlayState::Stopped){
		MetaData md(_play_manager->get_cur_track());
		track_changed(md);
	}

	playstate_changed(_play_manager->get_play_state());

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

	LibraryPluginHandler* lph = LibraryPluginHandler::getInstance();
	LibraryContainerInterface* current_library = lph->current_library();
	if(current_library){
		QWidget* current_library_widget = current_library->widget();

		if(current_library_widget == nullptr) return;

		current_library_widget->show();
		current_library_widget->resize(library_widget->size());
	}

	if(_settings->get(Set::Player_StartInTray)){
		this->setHidden(true);
	}

	connect(_pph, &PlayerPluginHandler::sig_show_plugin, this, &GUI_Player::show_plugin);
	connect(_pph, &PlayerPluginHandler::sig_hide_all_plugins, this, &GUI_Player::hide_all_plugins);

	QString shown_plugin = _settings->get(Set::Player_ShownPlugin);
	if(!shown_plugin.isEmpty()){
		PlayerPluginInterface* p  = _pph->find_plugin(shown_plugin);
		show_plugin(p);
	}
}

void GUI_Player::play_error(const QString& message)
{
	MetaData md = _play_manager->get_cur_track();
	QString err = message + "\n\n" + md.filepath();
	Message::warning(err, "Player");
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

	sp_log(Log::Info) << "Newest Version: " << new_version;
	sp_log(Log::Info) << "This Version:   " << cur_version;

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


void GUI_Player::really_close()
{
	sp_log(Log::Info) << "closing player...";

	QMainWindow::close();

	emit sig_player_closed();
}


