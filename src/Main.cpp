/* Main.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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

/*
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: luke
 */
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <fcntl.h>
#include <stdio.h>

#include "Application/application.h"
#include "Database/DatabaseConnector.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/EqualizerPresets.h"

#include "GUI/Helper/Shortcuts/ShortcutHandler.h"

#include <QSharedMemory>
#include <QTranslator>
#include <QFontDatabase>
#include <algorithm>
#include <type_traits>

#include <QtGlobal>
#ifdef Q_OS_LINUX
	#include <execinfo.h>
#endif


int check_for_another_instance_unix() {

#ifdef Q_OS_LINUX

	int pid = -1;

	QDir dir("/proc");
	dir.cd(".");
	QStringList lst = dir.entryList(QDir::Dirs);
	int n_instances = 0;

	for(const QString& dirname : lst) {
		bool ok;
		int tmp_pid = dirname.toInt(&ok);
		if(!ok) continue;

		dir.cd(dirname);

		QFile f(dir.absolutePath() + QDir::separator() + "cmdline");
		f.open(QIODevice::ReadOnly);
		if(!f.isOpen()) {
			dir.cd("..");
			continue;
		}

		QString str = f.readLine();
		f.close();

		if(str.contains("sayonara", Qt::CaseInsensitive)) {

			n_instances++;

			if(pid == -1 || tmp_pid < pid) {
				pid = tmp_pid;
			}

			if(n_instances > 1) {
				return pid;
			}
		}

		dir.cd("..");
	}

#endif
	return 0;
}


void printHelp() {
	sp_log(Log::Info) << "sayonara <list>";
	sp_log(Log::Info) << "<list> can consist of either files or directories or both";
}


void segfault_handler(int sig){

	Q_UNUSED(sig)

#ifdef Q_OS_LINUX

	void* array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 20);
	backtrace_symbols_fd(array, size, STDERR_FILENO);

#endif

}

/*
 * set->register_setting(new Setting<decltype(Lib_Path.p)>(Set::Lib_Path, "lib_path", "/home/") )
 *
 */

#define REGISTER_SETTING(key, db_key, def) set->register_setting( new Setting<std::remove_pointer<decltype(key.p)>::type>(key, db_key, def) )
#define REGISTER_SETTING_NO_DB(key, def) set->register_setting( new Setting<std::remove_pointer<decltype(key.p)>::type>(key, def) )
#include "Helper/Settings/SettingKey.h"
bool register_settings(){

	if(!DatabaseConnector::getInstance()->is_initialized()){
		return false;
	}

	BoolList shown_cols;
	for(int i=0; i<10; i++){
		shown_cols << true;
	}

	Settings* set = Settings::getInstance();

	REGISTER_SETTING( Set::LFM_Login, "LastFM_login", StringPair("None", "None"));
	REGISTER_SETTING( Set::LFM_Active, "LastFM_active", false );
	REGISTER_SETTING( Set::LFM_Corrections, "lfm_corrections", false );
	REGISTER_SETTING( Set::LFM_ShowErrors, "lfm_q.show_errors", false );
	REGISTER_SETTING( Set::LFM_SessionKey, "lfm_session_key", QString() );

	REGISTER_SETTING( Set::Eq_Last, "eq_last", 0);
	REGISTER_SETTING( Set::Eq_List, "EQ_list", EQ_Setting::get_defaults() );
	REGISTER_SETTING( Set::Eq_Gauss, "EQ_Gauss", true );

	REGISTER_SETTING( Set::Lib_ColsTitle, "lib_shown_cols_title", shown_cols );
	REGISTER_SETTING( Set::Lib_ColsArtist, "lib_shown_cols_artist", shown_cols );
	REGISTER_SETTING( Set::Lib_ColsAlbum, "lib_shown_cols_album", shown_cols );
	REGISTER_SETTING( Set::Lib_LiveSearch, "lib_live_search", true );
	REGISTER_SETTING( Set::Lib_Sorting, "lib_sortings", LibSortOrder() );
	REGISTER_SETTING( Set::Lib_Path, "library_path", QString() );
	REGISTER_SETTING( Set::Lib_Show, "show_library", true );
	REGISTER_SETTING( Set::Lib_CurPlugin ,"last_lib_plugin", "local_library");
	REGISTER_SETTING( Set::Lib_SplitterStateArtist ,"splitter_state_artist", QByteArray());
	REGISTER_SETTING( Set::Lib_SplitterStateGenre ,"splitter_state_genre", QByteArray());
	REGISTER_SETTING( Set::Lib_SplitterStateTrack ,"splitter_state_track", QByteArray());
	REGISTER_SETTING( Set::Lib_OldWidth ,"lib_old_width", 0);
	REGISTER_SETTING( Set::Lib_DC_DoNothing ,"lib_dc_do_nothing", true);
	REGISTER_SETTING( Set::Lib_DC_PlayIfStopped ,"lib_dc_play_if_stopped", false);
	REGISTER_SETTING( Set::Lib_DC_PlayImmediately ,"lib_dc_play_immediately", false);
	REGISTER_SETTING( Set::Lib_DD_DoNothing ,"lib_dd_do_nothing", true);
	REGISTER_SETTING( Set::Lib_DD_PlayIfStoppedAndEmpty ,"lib_dd_play_if_stopped_and_empty", false);
	REGISTER_SETTING( Set::Lib_FontSize ,"lib_font_size", -1);
	REGISTER_SETTING( Set::Lib_FontBold ,"lib_font_bold", true);

	REGISTER_SETTING( Set::Player_Version, "player_version", QString(SAYONARA_VERSION));
	REGISTER_SETTING( Set::Player_Language, "player_language", "sayonara_lang_en" );
	REGISTER_SETTING( Set::Player_Style, "player_style", 0 );
	REGISTER_SETTING( Set::Player_FontName, "player_font", QString() );
	REGISTER_SETTING( Set::Player_FontSize, "player_font_size", 10 );
	REGISTER_SETTING( Set::Player_Size, "player_size", QSize(800,600) );
	REGISTER_SETTING( Set::Player_Pos, "player_pos", QPoint(50,50) );
	REGISTER_SETTING( Set::Player_Fullscreen, "player_fullscreen", false );
	REGISTER_SETTING( Set::Player_Maximized, "player_maximized", false );
	REGISTER_SETTING( Set::Player_ShownPlugin, "shown_plugin", QString() );
	REGISTER_SETTING( Set::Player_OneInstance, "only_one_instance", true );
	REGISTER_SETTING( Set::Player_Min2Tray, "min_to_tray", false );
	REGISTER_SETTING( Set::Player_StartInTray, "start_in_tray", false );
	REGISTER_SETTING( Set::Player_NotifyNewVersion, "notify_new_version", true );
	REGISTER_SETTING( Set::Player_SplitterState ,"splitter_state_player", QByteArray());
	REGISTER_SETTING( Set::Player_Shortcuts, "shortcuts", RawShortcutMap());

	REGISTER_SETTING( Set::PL_Playlist, "playlist", QStringList() );
	REGISTER_SETTING( Set::PL_LoadSavedPlaylists, "load_saved_playlists", false );
	REGISTER_SETTING( Set::PL_LoadTemporaryPlaylists, "load_temporary_playlists", false );
	REGISTER_SETTING( Set::PL_LoadLastTrack, "load_last_track", false );
	REGISTER_SETTING( Set::PL_RememberTime, "remember_time", false );
	REGISTER_SETTING( Set::PL_StartPlaying, "start_playing", false );
	REGISTER_SETTING( Set::PL_LastTrack, "last_track", -1 );
	REGISTER_SETTING( Set::PL_LastPlaylist, "last_playlist", -1 );
	REGISTER_SETTING( Set::PL_Mode, "playlist_mode", PlaylistMode() );
	REGISTER_SETTING( Set::PL_ShowNumbers, "show_playlist_numbers", true );
	REGISTER_SETTING( Set::PL_EntryLook, "playlist_look", QString("*%title%* - %artist%"));
	REGISTER_SETTING( Set::PL_FontSize, "playlist_font_size", -1);

	REGISTER_SETTING( Set::Notification_Show, "show_notifications", true );
	REGISTER_SETTING( Set::Notification_Timeout, "notification_timeout", 5000 );
	REGISTER_SETTING( Set::Notification_Name, "notification_name", "DBus" );

	REGISTER_SETTING( Set::Engine_Name, "sound_engine", QString() );
	REGISTER_SETTING( Set::Engine_CurTrackPos_s, "last_track_pos", 0 );
	REGISTER_SETTING( Set::Engine_Vol, "volume", 50 );
	REGISTER_SETTING( Set::Engine_Mute, "mute", false );
	REGISTER_SETTING( Set::Engine_ConvertQuality, "convert_quality", 0 );
	REGISTER_SETTING( Set::Engine_CovertTargetPath, "convert_target_path", QDir::homePath() );
	REGISTER_SETTING( Set::Engine_Gapless, "gapless_playback", false);
	REGISTER_SETTING( Set::Engine_ShowLevel, "show_level", false);
	REGISTER_SETTING( Set::Engine_ShowSpectrum, "show_spectrum", false);
	REGISTER_SETTING( Set::Engine_SR_Active, "streamripper", false );
	REGISTER_SETTING( Set::Engine_SR_Warning, "streamripper_warning", true );
	REGISTER_SETTING( Set::Engine_SR_Path, "streamripper_path", QDir::homePath() );
	REGISTER_SETTING( Set::Engine_SR_SessionPath, "streamripper_session_path", true );

	REGISTER_SETTING( Set::Spectrum_Style, "spectrum_style", 0 );
	REGISTER_SETTING( Set::Level_Style, "level_style", 0 );

	REGISTER_SETTING( Set::Broadcast_Active, "broadcast_active", false );
	REGISTER_SETTING( Set::Broadcast_Prompt, "broadcast_prompt", false );
	REGISTER_SETTING( Set::Broadcast_Port, "broadcast_port", 54054 );

	REGISTER_SETTING( Set::Remote_Active, "remote_control_active", false);
	REGISTER_SETTING( Set::Remote_Port, "remote_control_port", 54055);




	REGISTER_SETTING_NO_DB( SetNoDB::MP3enc_found, true );
	REGISTER_SETTING_NO_DB( SetNoDB::Player_Quit, false );



	return set->check_settings();
}

QString get_current_locale_string(){
	QString lang_two = QString("sayonara_lang_") + QLocale::system().name().left(2).toLower() + ".qm";
	QString lang_four = QString("sayonara_lang_") + QLocale::system().name().toLower() + ".qm";

	if(QFile::exists(Helper::get_share_path() + lang_two)){
		return lang_two;
	}

	if(QFile::exists(Helper::get_share_path() + lang_four)){
		return lang_four;
	}

	return QString();
}

int main(int argc, char *argv[]) {

	Application app(argc, argv);

	bool success;
	QTranslator translator;
	QString language;
	QStringList files_to_play;

#ifdef Q_OS_LINUX

	signal(SIGSEGV, segfault_handler);

#endif

	bool single_instance=true;
	/* Init files to play in argument list */
	for(int i=1; i<argc; i++) {
		QString str(argv[i]);

		if(str.compare("--multi-instances") == 0){
			single_instance = false;
		}

		else
		{
			files_to_play << Helper::File::get_absolute_filename(QString(argv[i]));
		}
	}

#ifdef Q_OS_LINUX

	int pid=0;
	if(single_instance){
		pid = check_for_another_instance_unix();
	}

	if(pid > 0) {
		QSharedMemory memory("SayonaraMemory");

		if(!files_to_play.isEmpty()){

			QString filename = files_to_play[0] + "\n";
			QByteArray arr = filename.toUtf8();


			memory.attach(QSharedMemory::ReadWrite);

			if(memory.create(arr.size())){
				memory.lock();
				char* ptr = (char*) memory.data();
				int size = std::min(memory.size(), arr.size());

				memcpy(ptr,
					   arr.data(),
					   size);

				memory.unlock();
			}

			kill(pid, SIGUSR1);
		}

		else{
			kill(pid, SIGUSR2);
		}

		Helper::sleep_ms(500);

		if(memory.isAttached()){
			memory.detach();
		}

		sp_log(Log::Info) << "another instance is already running";

		return 0;
	}

#endif

	/* Tell the settings manager which settings are neccessary */
	if(!register_settings()){
		sp_log(Log::Error) << "Cannot initialize settings";
		return 1;
	}

	success = DatabaseConnector::getInstance()->load_settings();
	if(!success) {
		sp_log(Log::Error) << "Database: Could not load settings";
		return 1;
	}

	Q_INIT_RESOURCE(Icons);

	if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")) {
		QDir().mkdir(QDir::homePath() + QDir::separator() +  "/.Sayonara");
	}

	language = Settings::getInstance()->get(Set::Player_Language);
	/*if(language.compare("default") == 0){
		language = get_current_locale_string();
		if(language.isEmpty()){
			language = "sayonara_lang_en.qm";
		}
	}*/

	translator.load(language, Helper::get_share_path() + "translations");

	if(!app.init(&translator, files_to_play)) {
		return 1;
	}

	app.setApplicationName("Sayonara");
	app.setWindowIcon(Helper::get_icon("logo"));

	app.exec();

	return 0;
}
