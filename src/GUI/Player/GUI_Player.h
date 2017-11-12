/* GUI_Simpleplayer.h */

/* Copyright (C) 2011-2017 Lucio Carreras
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

#include "Components/PlayManager/PlayState.h"

#include "Utils/Message/GlobalMessageReceiverInterface.h"
#include "GUI/Utils/Widgets/Widget.h"

#include <QSystemTrayIcon>

class GUI_TrayIcon;
class MetaData;
class PreferenceDialog;
class QTranslator;
class QMessageBox;
class GUI_Logger;

namespace PlayerPlugin
{
	class Base;
	class Handler;
}

class GUI_Player :
		public Gui::MainWindow,
		public GlobalMessageReceiverInterface,
		private Ui::Sayonara
{
	Q_OBJECT

signals:
	void sig_player_closed();

public:
	explicit GUI_Player(QTranslator* translator, QWidget *parent=nullptr);
	~GUI_Player();

	void register_player_plugin_handler(PlayerPlugin::Handler* pph);
	void register_preference_dialog(PreferenceDialog* dialog);

	void ui_loaded();


private:
	PlayerPlugin::Handler*		_pph=nullptr;

	QTranslator*				_translator=nullptr;
	QStringList					_translators;

	QMessageBox*				_about_box=nullptr;
	GUI_Logger*					_logger=nullptr;

	QAction*					_action_shutdown=nullptr;


private:
	void init_gui();

	void setup_tray_actions ();
	void setup_volume_button(int percent);
	void setup_connections();

	void closeEvent(QCloseEvent* e) override;
	void keyPressEvent(QKeyEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;
	void moveEvent(QMoveEvent* e) override;

	void language_changed() override;
	void skin_changed() override;

	void set_total_time_label(int64_t length_ms);
	void set_cur_pos_label(int val);
	void set_cover_location(const MetaData& md);
	void set_standard_cover();

	// Methods for other mudules to display info/warning/error
	GlobalMessage::Answer error_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer warning_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer info_received(const QString &error, const QString &sender_name=QString()) override;
	GlobalMessage::Answer question_received(const QString &info, const QString &sender_name=QString(), GlobalMessage::QuestionType type=GlobalMessage::QuestionType::YesNo) override;


private slots:
	void playstate_changed(PlayState state);
	void play_error(const QString& message);

	/* File */
	void open_files_clicked();
	void open_dir_clicked();
	void shutdown_clicked();


	/* View */
	void show_library(bool);
	void show_fullscreen_toggled(bool);
	void skin_toggled(bool);

	void main_splitter_moved(int pos, int idx);

	void current_library_changed(const QString& name);
	void check_library_menu_action();

	void about();
	void help();

	void awa_version_finished();
	void awa_translators_finished();

	void really_close();

	void tray_icon_activated(QSystemTrayIcon::ActivationReason reason);

	/* Plugins */
	void plugin_opened();
	void plugin_closed();
	void plugin_action_triggered(bool b);
};

#endif // GUI_SIMPLEPLAYER_H
