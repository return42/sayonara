/* PlaylistChooser.h */

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

#ifndef PLAYLISTCHOOSER_H_
#define PLAYLISTCHOOSER_H_

#include "Helper/Playlist/CustomPlaylistFwd.h"
#include "Helper/Pimpl.h"
#include <QObject>

class PlaylistDBWrapper;
class PlaylistHandler;
class QString;
class QStringList;

/**
 * @brief The PlaylistChooser class
 * @ingroup Components
 */
class PlaylistChooser :
        public QObject
{
	Q_OBJECT
    PIMPL(PlaylistChooser)

public:
	PlaylistChooser();
	~PlaylistChooser();

	void delete_playlist(int id);

	void load_single_playlist(int id);

	void save_playlist(int id);
	void save_playlist(const QString& playlist_name);
	void save_playlist_file(const QString& filename, bool);
	void clear_playlist();
	void playlist_files_selected(const QStringList& lst);
	int find_playlist(const QString& name) const;

signals:
	void sig_all_playlists_loaded(const CustomPlaylistSkeletons&);

public slots:
	void load_all_playlists();
};

#endif /* PLAYLISTS_H_ */
