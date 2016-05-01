/* GUI_Player.cpp */

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



#include "GUI_Player.h"
#include "GUI_TrayIcon.h"
#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"
#include "GUI/Playlist/GUI_Playlist.h"
#include "GUI/Helper/Message/Message.h"
#include "GUI/Helper/IconLoader/IconLoader.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/Style/Style.h"
#include "GUI/MTP/GUI_MTP.h"

#include "Helper/Helper.h"
#include "Helper/globals.h"

#include "Helper/FileHelper.h"
#include "Helper/WebAccess/AsyncWebAccess.h"

#include "Components/CoverLookup/CoverLookup.h"
#include "Components/Library/LocalLibrary.h"
#include "Components/Engine/EngineHandler.h"

#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"
#include "Interfaces/PlayerPlugin/PlayerPluginHandler.h"
#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Interfaces/PreferenceDialog/PreferenceDialogInterface.h"



#include <QFileDialog>
#include <QPalette>

GUI_Player::GUI_Player(QTranslator* translator, QWidget *parent) :
	SayonaraMainWindow(parent),
	SayonaraShortcutWidget(),
	GlobalMessageReceiverInterface("Player Main Window"),
	Ui::Sayonara()
{
	setupUi(this);

	GlobalMessage::getInstance()->register_receiver(this);

	init_gui();

	_translator = translator;
	_engine = EngineHandler::getInstance();
	_play_manager = PlayManager::getInstance();
	_icon_loader = IconLoader::getInstance();

	set_cur_pos_ms(_play_manager->get_init_position_ms());

	QString version = _settings->get(Set::Player_Version);

#ifdef WITH_MTP
	_mtp = new GUI_MTP(this);
	action_devices->setVisible(true);
#else
	action_devices->setVisible(false);
#endif

	lab_sayonara->setText(tr("Sayonara Player"));
	lab_version->setText( version );
	lab_writtenby->setText(tr("Written by") + " Lucio Carreras");
	lab_copyright->setText(tr("Copyright") + " 2011-" + QString::number(QDateTime::currentDateTime().date().year()));

	progress_widget->setCurrentIndex(0);

	_cov_lookup = new CoverLookup(this);
	_ui_alternative_covers = new GUI_AlternativeCovers(this->centralWidget());

	init_action(action_viewLibrary, Set::Lib_Show);
	init_action(action_livesearch, Set::Lib_LiveSearch);
	init_action(action_Dark, Set::Player_Style);

	bool show_library = _settings->get(Set::Lib_Show);
	this->show_library(show_library);

	int volume = _play_manager->get_volume();
	volume_changed(volume);

	bool muted = _play_manager->get_mute();
	mute_changed(muted);

	/* tray actions */
	setup_tray_actions();

	// signals and slots
	setup_connections();



	plugin_widget->resize(plugin_widget->width(), 0);

	REGISTER_LISTENER(Set::Engine_SR_Active, _sl_sr_active_changed);

	REGISTER_LISTENER_NO_CALL(Set::Player_Fullscreen, _sl_fullscreen_toggled);
	REGISTER_LISTENER_NO_CALL(SetNoDB::Player_Quit, really_close);

	REGISTER_LISTENER(Set::Player_FontName, skin_changed);
	REGISTER_LISTENER(Set::Player_FontSize, skin_changed);
	REGISTER_LISTENER(Set::PL_FontSize, skin_changed);
	REGISTER_LISTENER(Set::Lib_FontSize, skin_changed);
	REGISTER_LISTENER(Set::Lib_FontBold, skin_changed);
}


GUI_Player::~GUI_Player() {

	sp_log(Log::Info) << "closing player...";
}


void GUI_Player::language_changed() {

	QString language = _settings->get(Set::Player_Language);
	_translator->load(language, Helper::get_share_path() + "translations/");

	retranslateUi(this);
}


void GUI_Player::init_gui() {

	btn_mute->setIcon(GUI::get_icon("vol_1"));

	action_viewLibrary->setText(tr("Library"));
	btn_rec->setVisible(false);

	action_Fullscreen->setShortcut(QKeySequence("F11"));
	action_Dark->setShortcut(QKeySequence("F10"));

}


// new track
void GUI_Player::track_changed(const MetaData & md) {

	_md = md;

	lab_sayonara->hide();
	lab_title->show();

	lab_version->hide();
	lab_artist->show();

	lab_writtenby->hide();
	lab_album->show();

	lab_copyright->hide();
	lab_rating->show();

	set_info_labels();
	set_cur_pos_label(0);
	set_total_time_label(_md.length_ms);

	QString rating_text;


	if(_md.bitrate / 1000 > 0){
		rating_text = QString::number(_md.bitrate / 1000) + " kBit/s";
	}

	if(_md.filesize > 0){
		if(_md.bitrate / 1000 > 0){
			rating_text += ", ";
		}
		rating_text += QString::number( (double) (_md.filesize / 1024) / 1024.0, 'f', 2) + " MB";
	}

	lab_rating->setText(rating_text);
	lab_rating->setToolTip(rating_text);

	set_radio_mode( _md.radio_mode() );

	this->setWindowTitle(QString("Sayonara - ") + md.title);
	this->repaint();

	fetch_cover();
}

void GUI_Player::set_title_label(){

	QString text = elide_text(_md.title, lab_title, 2);
	lab_title->setText(text);
}

void GUI_Player::set_info_labels()
{
	set_title_label();
	set_album_label();
	set_artist_label();
}

void GUI_Player::set_artist_label(){

	QFontMetrics fm = lab_artist->fontMetrics();
	lab_artist->setText( fm.elidedText(_md.artist, Qt::ElideRight, lab_artist->width()) );
}

void GUI_Player::set_album_label(){

	QString str_year = QString::number(_md.year);
	QString album_name = _md.album;
	QFontMetrics fm = lab_album->fontMetrics();

	if(_md.year > 1000 && !_md.album.contains(str_year)){
		album_name += " (" + str_year + ")";
	}

	lab_album->setText( fm.elidedText(album_name, Qt::ElideRight, lab_album->width()) );
}

void GUI_Player::dur_changed(const MetaData& md) {

	if(_md != md){
		return;
	}

	set_total_time_label(md.length_ms);
}

void GUI_Player::br_changed(const MetaData& md) {

	QString rating_text;

	if(_md != md){
		return;
	}

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

void GUI_Player::md_changed(const MetaData& md) {

	if(_md != md && md.title == _md.title){
		return;
	}

	md.print();

	_md = md;
	_md.title = md.title;

	lab_title->setText( md.title );
}


// public slot:
// id3 tags have changed
void GUI_Player::id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new) {

	IdxList idxs = v_md_old.findTracks(_md.filepath());
	if(idxs.isEmpty()) return;

	int idx = idxs[0];

	_md = v_md_new[idx];

	set_info_labels();

	setWindowTitle(QString("Sayonara - ") + _md.title);
	fetch_cover();
}


void GUI_Player::skin_toggled(bool on){

	_settings->set(Set::Player_Style, (on ? 1 : 0) );
}

void GUI_Player::skin_changed() {

	bool dark = (_settings->get(Set::Player_Style) == 1);

	QString stylesheet = Style::get_style(dark);

	this->setStyleSheet(stylesheet);

	btn_fw->setIcon(_icon_loader->get_icon("media-skip-forward", "fwd"));
	btn_bw->setIcon(_icon_loader->get_icon("media-skip-backward", "bwd"));

	if(_play_manager->get_play_state() == PlayManager::PlayState::Playing){
		btn_play->setIcon(_icon_loader->get_icon("media-playback-pause", "pause"));
	}

	else{
		btn_play->setIcon(_icon_loader->get_icon("media-playback-start", "play"));
	}


	btn_stop->setIcon(_icon_loader->get_icon("media-playback-stop", "stop"));
	btn_rec->setIcon(_icon_loader->get_icon("media-record", "rec"));

	action_OpenFile->setIcon(_icon_loader->get_icon("document-open", "play"));
	action_OpenFolder->setIcon(_icon_loader->get_icon("document-open", "play"));
	action_Close->setIcon(_icon_loader->get_icon("window-close", "power_off"));

	setup_volume_button(sli_volume->value());
}

void GUI_Player::reload_skin(){
	skin_changed();
}

/** TRAY ICON **/
void GUI_Player::setup_tray_actions() {

	_tray_icon = new GUI_TrayIcon(this);
	_tray_icon->installEventFilter(this);

	connect(_tray_icon, &GUI_TrayIcon::sig_close_clicked, this, &GUI_Player::really_close);
	connect(_tray_icon, &GUI_TrayIcon::sig_show_clicked, this, &GUI_Player::raise);
	connect(_tray_icon, &GUI_TrayIcon::sig_wheel_changed, this, &GUI_Player::change_volume_by_tick);
	connect(_tray_icon, &GUI_TrayIcon::activated, this, &GUI_Player::tray_icon_activated);

	_tray_icon->show();
}


void GUI_Player::tray_icon_activated (QSystemTrayIcon::ActivationReason reason) {

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

	case QSystemTrayIcon::MiddleClick:
		break;

	default:
		break;
	}
}



void GUI_Player::set_libraries(LibraryPluginHandler* plugin_loader) {

	_lph = plugin_loader;

	plugin_loader->set_library_parent(library_widget);
	check_library_menu_action();

	connect(_lph, &LibraryPluginHandler::sig_idx_changed, this, &GUI_Player::library_idx_changed);
}

void GUI_Player::library_idx_changed(int idx){
	Q_UNUSED(idx);
	check_library_menu_action();
}

void GUI_Player::check_library_menu_action()
{
	QList<LibraryContainerInterface*> libraries;
	bool library_visible;
	int cur_idx;

	if(!_lph){
		return;
	}

	libraries = _lph->get_libraries();
	library_visible = _settings->get(Set::Lib_Show);
	cur_idx = _lph->get_cur_library_idx();

	for(int i=0; i<libraries.size(); i++){

		QMenu* menu;
		QAction* action;
		LibraryContainerInterface* library;

		library = libraries[i];
		if(!library){
			continue;
		}

		menu = library->get_menu();
		if(!menu){
			continue;
		}

		if(cur_idx == i && library_visible){
			action = menubar->insertMenu(menuAbout->menuAction(), menu);

			if(action){
				action->setText(library->get_display_name());
			}

			library->set_menu_action(action);
		}

		else {
			action = library->get_menu_action();
			if(action){
				menubar->removeAction(action);
			}
		}
	}

}

void GUI_Player::set_player_plugin_handler(PlayerPluginHandler* pph) {

	_pph = pph;

	QList<PlayerPluginInterface*> lst = _pph->get_all_plugins();
	QList<QAction*> actions;

	int i=1;
	for(PlayerPluginInterface* p : lst) {
		QAction* action = p->get_action();
		QKeySequence ks("Ctrl+F" + QString::number(i));
		action->setShortcut(ks);
		action->setData(p->get_name());
		// action is connected in Plugin itself
		p->setParent(plugin_widget);
		actions << action;
		i++;
	}

	menuView->insertActions(action_Dark, actions);
	menuView->insertSeparator(action_Dark);

	connect(_pph, &PlayerPluginHandler::sig_show_plugin, this, &GUI_Player::show_plugin);
	connect(_pph, &PlayerPluginHandler::sig_hide_all_plugins, this, &GUI_Player::hide_all_plugins);
}


/** LIBRARY AND PLAYLIST END **/


void GUI_Player::register_preference_dialog(PreferenceDialogInterface* dialog){

	QList<QAction*> actions = menuFle->actions();
	QAction* sep = actions[actions.size() - 4];

	dialog->setParent(this);
	menuFle->insertAction(sep, dialog->get_action());

}

// prvt fct
void GUI_Player::set_radio_mode(RadioMode radio) {

	bool stream_ripper = _settings->get(Set::Engine_SR_Active);

	if(stream_ripper) {

		bool btn_rec_visible = (radio != RadioMode::Off);

		if(btn_rec_visible) {
			btn_play->setVisible(radio == RadioMode::Off);
			btn_rec->setVisible(radio != RadioMode::Off);
		}

		else {
			btn_rec->setVisible(radio != RadioMode::Off);
			btn_play->setVisible(radio == RadioMode::Off);
		}
	}

	else{

		btn_rec->setVisible(false);
		btn_play->setVisible(true);
	}

	_tray_icon->set_enable_fwd(true);
	sli_progress->setEnabled( (_md.length_ms / 1000) > 0 );

	_play_manager->record(btn_rec->isChecked() && btn_rec->isVisible());

}


// public slot
void GUI_Player::_sl_sr_active_changed() {

	bool active = _settings->get(Set::Engine_SR_Active);

	if(active) {
		btn_play->setVisible(_md.radio_mode() == RadioMode::Off);
		btn_rec->setVisible(_md.radio_mode() != RadioMode::Off);
	}

	else{
		btn_play->setVisible(true);
		btn_rec->setVisible(false);
	}

	btn_rec->setChecked(false);
}



void GUI_Player::ui_loaded() {

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


	QString shown_plugin = _settings->get(Set::Player_ShownPlugin);
	if(!shown_plugin.isEmpty()){
		PlayerPluginInterface* p  = _pph->find_plugin(shown_plugin);
		show_plugin(p);
	}

	if(_play_manager->get_play_state() != PlayManager::PlayState::Stopped){
		MetaData md = _play_manager->get_cur_track();
		track_changed(md);
	}

	playstate_changed(_play_manager->get_play_state());

	if(_settings->get(Set::Player_NotifyNewVersion)){
		AsyncWebAccess* awa = new AsyncWebAccess(this);
		awa->run("http://sayonara-player.com/current_version");
		connect(awa, &AsyncWebAccess::sig_finished, this, &GUI_Player::awa_version_finished);
	}

	QWidget* cur_library=nullptr;
	LibraryContainerInterface* cur_interface;
	cur_interface = _lph->get_cur_library();

	splitter->restoreState(_settings->get(Set::Player_SplitterState));
	splitter->layout()->update();
	splitter->update();

	if(cur_interface){
		cur_library = cur_interface->get_ui();

		if(cur_library == nullptr) return;

		cur_library->show();
		cur_library->resize(library_widget->size());
	}

	if(_settings->get(Set::Player_StartInTray)){
		this->setHidden(true);
	}
}



void GUI_Player::awa_version_finished(bool success) {

	if(!success) return;

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!awa){
		return;
	}

	QString new_version(awa->get_data());
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
}

void GUI_Player::awa_translators_finished(bool success) {

	if(!success) return;

	AsyncWebAccess* awa = static_cast<AsyncWebAccess*>(sender());
	if(!awa){
		return;
	}

	QString data(awa->get_data());
	QStringList translators = data.split('\n');

	_translators.clear();

	for(const QString& str : translators) {
		if(str.trimmed().size() > 0) {
			_translators.push_back(str);
		}
	}

	about();
}

void GUI_Player::notify_new_version_toggled(bool b) {
	_settings->set(Set::Player_NotifyNewVersion, b);
}

void GUI_Player::read_filelist(const QString& bla){

	Q_UNUSED(bla)

	sp_log(Log::Debug) << "Something happened in sayonara dir";

	QString filename = Helper::get_sayonara_path() + "sayonara_files";
	if(!QFile::exists(filename)){
		return;
	}


	bool success;
	QStringList files_to_play;
	QString content;
	PlaylistHandler* playlist_handler;
	QString playlist_name;



	success = Helper::File::read_file_into_str(filename, content);
	if(!success){
		return;
	}

	sp_log(Log::Debug) << "Read files...";
	files_to_play = content.split("\n");
	if(files_to_play.isEmpty()){
		return;
	}


	QFile f(filename);
	f.remove();

	playlist_handler = PlaylistHandler::getInstance();
	playlist_name = playlist_handler->request_new_playlist_name();
	sp_log(Log::Debug) << "Create new playlist: " << playlist_name;
	playlist_handler->create_playlist(files_to_play,
									  playlist_name,
									  true,
									  Playlist::Type::Std
									  );
}

void GUI_Player::raise(){

	QMainWindow::raise();

	if(this->isHidden())
	{
		this->setHidden(false);
	}

	if(!this->isVisible()){
		this->setVisible(true);
	}

	this->setWindowFlags((Qt::WindowFlags) (windowFlags() & ~Qt::WindowMinimized));
	this->activateWindow();
	this->show();
	//this->raise();
}

void GUI_Player::really_close() {

	_tray_icon->hide();
	_tray_icon->deleteLater();

	this->hide();

	QMainWindow::close();
}
