/* GUI_Simpleplayer.h */

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


#ifndef GUI_SIMPLEPLAYER_H
#define GUI_SIMPLEPLAYER_H

#include "GUI/Player/ui_GUI_Player.h"

#include "Components/PlayManager/PlayManager.h"
#include "Components/CoverLookup/CoverLocation.h"

#include "GUI/Player/GUI_TrayIcon.h"
#include "GUI/Helper/GlobalMessage/GlobalMessageReceiverInterface.h"



#include <QMessageBox>
#include <QMainWindow>
#include <QShowEvent>
#include <QCloseEvent>
#include <QTranslator>
#include <QKeySequence>
#include <QFileSystemWatcher>

#include <QAction>

class CoverLookup;
class LocalLibrary;
class GUI_AlternativeCovers;
class EngineHandler;
class GUI_Playlist;
class AsyncWebAccess;
class PlayerPluginInterface;
class PlayerPluginHandler;
class LibraryPluginHandler;
class PreferenceDialogInterface;
class IconLoader;

#ifdef WITH_MTP
	class GUI_MTP;
#endif

class GUI_Player :
		public SayonaraMainWindow,
		public SayonaraShortcutWidget,
		public GlobalMessageReceiverInterface,
		private Ui::Sayonara
{

	Q_OBJECT


public:

	GUI_Player(QTranslator* translator, QWidget *parent=nullptr);
    ~GUI_Player();

	// set the playlist ui

	void set_libraries(LibraryPluginHandler* plugin_loader);

	void set_player_plugin_handler(PlayerPluginHandler* pph);
	void register_preference_dialog(PreferenceDialogInterface* dialog);

	// called from signal handler
	// show if there are files to play
	void read_filelist(const QString& filename);

	void ui_loaded();
	QString get_shortcut_text(const QString &shortcut_identifier) const override;


public slots:

	void set_cur_pos_ms(quint64 pos_ms);
	void id3_tags_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new);

	void md_changed(const MetaData&);
	void dur_changed(const MetaData&);
	void br_changed(const MetaData&);

	void raise();
	void really_close();
	void reload_skin();

	void tray_icon_activated(QSystemTrayIcon::ActivationReason reason);

    /* Plugins */
	void show_plugin(PlayerPluginInterface* plugin);
	void hide_all_plugins();


private:

	QWidget*					_cur_library=nullptr;
	LocalLibrary*				_local_library=nullptr;

	GUI_AlternativeCovers*		_ui_alternative_covers=nullptr;

	EngineHandler*				_engine=nullptr;
	CoverLookup*				_cov_lookup=nullptr;
	PlayerPluginHandler*		_pph=nullptr;
	LibraryPluginHandler*		_lph=nullptr;

#ifdef WITH_MTP
	GUI_MTP*					_mtp=nullptr;
#endif

	GUI_TrayIcon*				_tray_icon=nullptr;


	QTranslator*				_translator=nullptr;
	QStringList					_translators;

	PlayManager*				_play_manager=nullptr;
	QMessageBox*				_about_box=nullptr;

	MetaData					_md;
	IconLoader*				_icon_loader=nullptr;


private:
	void init_gui();

	void setup_tray_actions ();
	void setup_volume_button(int percent);
	void setup_connections();

	void set_album_label();
	void set_artist_label();
	void set_title_label();
	void set_info_labels();

	void set_radio_mode(RadioMode model);

	bool check_library_path();

	void closeEvent(QCloseEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;
	void moveEvent(QMoveEvent* e) override;

	void language_changed() override;
	void skin_changed() override;

	void set_total_time_label(qint64 length_ms);
	void set_cur_pos_label(int val);

	template<typename T>
	void init_action(QAction* action, T setting_key){
		bool b = _settings->get(setting_key);
		action->setChecked(b);
	}


	// Methods for other mudules to display info/warning/error
	GlobalMessage::Answer error_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer warning_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer info_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer question_received(const QString &info, const QString &sender_name=QString(), GlobalMessage::QuestionType type=GlobalMessage::QuestionType::YesNo) override;




private slots:

	void play_clicked();
	void stop_clicked();
	void prev_clicked();
	void next_clicked();
	void rec_clicked(bool);
	void buffering(int progress);

	void played();
	void paused();
	void stopped();
	void playstate_changed(PlayManager::PlayState);

	void track_changed(const MetaData& md);

	void cover_clicked();
	void seek(int);
	void jump_forward();
	void jump_backward();
	void jump_forward_ms();
	void jump_backward_ms();

	void mute_button_clicked();
	void volume_slider_moved(int val);
	void volume_changed(int val);
	void mute_changed(bool mute);
	void change_volume_by_tick(int val);
	void increase_volume();
	void decrease_volume();

	/* File */
	void open_files_clicked();
	void open_dir_clicked();


	/* View */
	void show_library(bool);
	void show_notification_toggled(bool);
	void show_fullscreen_toggled(bool);
	void _sl_fullscreen_toggled();
	void skin_toggled(bool);

	void set_library_path_clicked();

	void min2tray_toggled(bool);
	void only_one_instance_toggled(bool);
	void live_search_toggled(bool);
	void notify_new_version_toggled(bool);

	void main_splitter_moved(int pos, int idx);

	void library_idx_changed(int idx);
	void check_library_menu_action();


	void about();
	void help();

	void set_standard_cover();
	void set_cover_image(const CoverLocation&);
	void fetch_cover();

	void awa_version_finished(bool success);
	void awa_translators_finished(bool success);

	void _sl_sr_active_changed();


};


#endif // GUI_SIMPLEPLAYER_H
