/* GUI_PlaylistChooser.h */

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

#ifndef GUIPLAYLISTCHOOSER_H_
#define GUIPLAYLISTCHOOSER_H_

#include "Interfaces/PlayerPlugin/PlayerPlugin.h"
#include "Helper/Playlist/CustomPlaylistFwd.h"

#include <QMap>
#include <QDockWidget>
#include <QInputDialog>
#include <QFileDialog>
#include <QPixmap>

class PlaylistChooser;

namespace Ui { class GUI_PlaylistChooser; }

class GUI_PlaylistChooser :
		public PlayerPluginInterface
{
	Q_OBJECT

private:

	Ui::GUI_PlaylistChooser*		ui=nullptr;
	QString							_last_dir;
	PlaylistChooser*				_playlist_chooser=nullptr;


public:
	explicit GUI_PlaylistChooser(QWidget *parent=nullptr);
	virtual ~GUI_PlaylistChooser();

	QString get_name() const override;
	QString get_display_name() const override;


public slots:
	void all_playlists_fetched(const CustomPlaylistSkeletons&);


private:
	void retranslate_ui() override;
	void init_ui() override;


private slots:
	void edit_text_changed(const QString& name);
	void save_button_pressed();
	void save_as_button_pressed();
	void delete_button_pressed();
	void load_button_pressed();
	void playlist_selected(int);

	void got_save_params(const QString&, bool);
};

#endif /* GUIPLAYLISTCHOOSER_H_ */
