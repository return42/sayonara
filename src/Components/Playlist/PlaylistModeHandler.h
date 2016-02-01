/* PlaylistModeHandler.h */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#ifndef _PLAYLIST_MODES_
#define _PLAYLIST_MODES_

#include "Helper/globals.h"
#include "Helper/PlaylistMode.h"

class PlaylistModeHandler : public QObject {

	Q_OBJECT
	SINGLETON(PlaylistModeHandler)

	private:

		PlaylistMode _mode;
		PlaylistMode _old_mode;

		void save_and_emit();

	signals:

		void sig_playlist_mode_changed(const PlaylistMode&);

	public slots:

		void enable_repeat();
		void disable_repeat();
		void toggle_repeat( bool );
		
		void enable_shuffle();
		void disable_shuffle();
		void toggle_shuffle( bool );

		void enable_append();
		void disable_append();
		void toggle_append( bool );


		void enable_dynamic();
		void disable_dynamic();
		void toggle_dynamic( bool );

		void enable_gapless();
		void disable_gapless();
		void toggle_gapless( bool );

		void set_mode(const PlaylistMode& mode);
};


#endif
