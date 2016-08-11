/*
 * GUI_PlayerCover.cpp
 *
 *  Created on: 07.12.2012
 *      Author: Lucio Carreras
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

#include "GUI/Helper/GUI_Helper.h"

#include <QImage>

/** COVERS **/

void GUI_Player::set_cover_location(){

	CoverLocation cl;
	if(_md.album_id >= 0) {
		cl = CoverLocation::get_cover_location(_md.album_id, _md.db_id);
	}

	else {
		cl = CoverLocation::get_cover_location( _md.album, _md.artist);
	}

	albumCover->set_cover_location(cl);
}

void GUI_Player::set_standard_cover() {

	albumCover->set_cover_location(CoverLocation::getInvalidLocation());
}

void GUI_Player::cover_changed(const QImage& img)
{
	_cover_from_tag = true;

	QPixmap pm = QPixmap::fromImage(img);
	QIcon icon(pm);
	albumCover->setIcon(icon);
}


/** COVER END **/

