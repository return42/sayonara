
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




#ifndef PLAYLISTBOTTOMBAR_H
#define PLAYLISTBOTTOMBAR_H

#include "Helper/Shutdown/Shutdown.h"
#include "Helper/PlaylistMode.h"

#include "GUI/Helper/SayonaraWidget.h"
#include "GUI/Playlist/PlaylistMenu.h"
#include "GUI/Playlist/ui_GUI_PlaylistBottomBar.h"

#ifdef WITH_SHUTDOWN
	#include "GUI/ShutdownDialog/GUI_Shutdown.h"
#endif

#include "Components/Playlist/Playlist.h"

class GUI_PlaylistBottomBar :
		public SayonaraWidget,
		private Ui::GUI_PlaylistBottomBar
{
	Q_OBJECT

signals:
	void sig_show_numbers_changed(bool active);
	void sig_playlist_mode_changed(const PlaylistMode& mode);

public:
	explicit GUI_PlaylistBottomBar(QWidget *parent = 0);
	virtual ~GUI_PlaylistBottomBar();

	void check_dynamic_play_button();
	void set_playlist_type(Playlist::Type playlist_type);

private slots:
	void btn_menu_pressed(QPoint pos);

#ifdef WITH_SHUTDOWN
	void shutdown_toggled(bool b);
	void shutdown_closed();
#endif

	void rep1_checked(bool checked);
	void rep_all_checked(bool checked);
	void shuffle_checked(bool checked);
	void playlist_mode_changed();
	void _sl_playlist_mode_changed();


private:
	PlaylistMode		_plm;
	PlaylistMenu*		_playlist_menu=nullptr;

#ifdef WITH_SHUTDOWN
	GUI_Shutdown*		_ui_shutdown=nullptr;
#endif
};



#endif // PLAYLISTBOTTOMBAR_H
