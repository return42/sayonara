
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



#ifndef GUI_PLAYLISTENTRYLOOK_H
#define GUI_PLAYLISTENTRYLOOK_H

#include "GUI/Helper/SayonaraDialog.h"
#include "GUI/Playlist/ui_GUI_PlaylistEntryLook.h"

class GUI_PlaylistEntryLook :
		public SayonaraDialog,
		protected Ui::GUI_PlaylistLook
{

	Q_OBJECT

public:
	GUI_PlaylistEntryLook(QWidget* parent=nullptr);

public slots:
	void commit();
	void revert();
	void revert_and_close();

protected:
	void language_changed() override;
};


#endif // GUI_PLAYLISTENTRYLOOK_H
