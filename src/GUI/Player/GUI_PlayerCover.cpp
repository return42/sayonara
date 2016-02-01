/*
 * GUI_PlayerCover.cpp
 *
 *  Created on: 07.12.2012
 *      Author: luke
 */

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

#include "GUI/AlternativeCovers/GUI_AlternativeCovers.h"

#include "Components/CoverLookup/CoverLookup.h"


/** COVERS **/

void GUI_Player::fetch_cover() {

	CoverLocation cover_location = CoverLocation::get_cover_location(_md);
	_cov_lookup->fetch_cover(cover_location);
}


void GUI_Player::cover_clicked() {

	if(_md.album_id >= 0) {
	   _ui_alternative_covers->start(_md.album_id, _md.db_id);
    }

    else {
		_ui_alternative_covers->start( _md.album, _md.artist);
    }

    this->setFocus();
}


void GUI_Player::set_standard_cover() {

	QIcon icon = Helper::get_icon("logo.png");

	albumCover->icon().detach();
	albumCover->setIcon(icon);
	albumCover->repaint();
}


void GUI_Player::set_cover_image(const CoverLocation& cl) {

	QIcon icon(cl.cover_path);
	albumCover->setIcon(icon);
}


/** COVER END **/

