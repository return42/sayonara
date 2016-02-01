/* GUILibraryInfoBox.cpp

 * Copyright (C) 2012  
 *
 * This file is part of sayonara-player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * created by Lucio Carreras, 
 * Sep 2, 2012 
 *
 */

#include "GUI_LibraryInfoBox.h"

#include "Database/DatabaseConnector.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

#include <QMap>
#include <QPixmap>


GUI_LibraryInfoBox::GUI_LibraryInfoBox(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::GUI_LibraryInfoBox()
{
	setupUi(this);

	_db = DatabaseConnector::getInstance();

    hide();
}

GUI_LibraryInfoBox::~GUI_LibraryInfoBox() {

}



void GUI_LibraryInfoBox::language_changed() {

	retranslateUi(this);
}


void GUI_LibraryInfoBox::psl_refresh() {

    MetaDataList v_md;
	AlbumList v_albums;
	ArtistList v_artists;

	_db->getTracksFromDatabase(v_md);
	_db->getAllAlbums(v_albums);
	_db->getAllArtists(v_artists);

	_n_tracks = v_md.size();
	_n_albums = v_albums.size();
	_n_artists = v_artists.size();
	_duration_ms = 0;
    _filesize = 0;

	for( const MetaData& md : v_md ) {
		_duration_ms += md.length_ms;
        _filesize += md.filesize;
	}

	_duration_string = Helper::cvt_ms_to_string(_duration_ms, false);
	_filesize_str = FileHelper::calc_filesize_str(_filesize);


	lab_album_count->setText(QString::number(_n_albums));
	lab_track_count->setText(QString::number(_n_tracks));
	lab_artist_count->setText(QString::number(_n_artists));
	lab_duration_value->setText(_duration_string + "s");
	lab_filesize->setText(_filesize_str);

	show();
}



