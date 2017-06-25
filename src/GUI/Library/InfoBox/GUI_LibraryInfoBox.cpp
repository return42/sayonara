/* GUILibraryInfoBox.cpp

 * Copyright (C) 2011-2017 Lucio Carreras
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
#include "GUI/Library/ui_GUI_LibraryInfoBox.h"

#include "Components/Library/LibraryManager.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"

#include "Helper/FileHelper.h"
#include "Helper/Helper.h"
#include "Helper/Language.h"
#include "Helper/Library/LibraryInfo.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/MetaData/MetaDataList.h"

#include "GUI/Helper/IconLoader/IconLoader.h"

#include <QMap>
#include <QPixmap>


GUI_LibraryInfoBox::GUI_LibraryInfoBox(qint8 library_id, QWidget* parent) :
	SayonaraDialog(parent)
{
	ui = new Ui::GUI_LibraryInfoBox();
	ui->setupUi(this);

	_library_id = library_id;

	hide();

	skin_changed();
	language_changed();
}

GUI_LibraryInfoBox::~GUI_LibraryInfoBox() {}

void GUI_LibraryInfoBox::language_changed()
{
	ui->retranslateUi(this);

	ui->lab_tracks->setText(Lang::get(Lang::Tracks).toFirstUpper());
	ui->lab_artists->setText(Lang::get(Lang::Artists));
	ui->lab_albums->setText(Lang::get(Lang::Albums));
	ui->lab_duration->setText(Lang::get(Lang::Duration));
	ui->lab_filesize_descr->setText(Lang::get(Lang::Filesize));
	ui->btn_close->setText(Lang::get(Lang::Close));

	ui->lab_path->setText(
		LibraryManager::getInstance()->library_info(_library_id).path()
	);

	this->setWindowTitle(Lang::get(Lang::Info));
}

void GUI_LibraryInfoBox::skin_changed()
{
	IconLoader* icon_loader = IconLoader::getInstance();

	QSize sz = ui->lab_icon->size();
	QPixmap pm = icon_loader->get_icon("dialog-inforrr", "info").pixmap(sz, QIcon::Normal, QIcon::On);

	ui->lab_icon->setPixmap(pm);
}


void GUI_LibraryInfoBox::psl_refresh()
{
	DatabaseConnector* db = DatabaseConnector::getInstance();
	LibraryDatabase* lib_db = db->library_db(_library_id, 0);

	MetaDataList v_md;
	AlbumList v_albums;
	ArtistList v_artists;

	lib_db->getAllTracks(v_md);
	lib_db->getAllAlbums(v_albums);
	lib_db->getAllArtists(v_artists);

	int n_tracks = v_md.size();
	int n_albums = v_albums.size();
	int n_artists = v_artists.size();
	qint64 duration_ms = 0;
	qint64 filesize = 0;

	for( const MetaData& md : v_md ) {
		duration_ms += md.length_ms;
		filesize += md.filesize;
	}

	QString duration_string = Helper::cvt_ms_to_string(duration_ms, false);
	QString filesize_str = Helper::File::calc_filesize_str(filesize);

	ui->lab_album_count->setText(QString::number(n_albums));
	ui->lab_track_count->setText(QString::number(n_tracks));
	ui->lab_artist_count->setText(QString::number(n_artists));
	ui->lab_duration_value->setText(duration_string + "s");
	ui->lab_filesize->setText(filesize_str);

	show();
}
