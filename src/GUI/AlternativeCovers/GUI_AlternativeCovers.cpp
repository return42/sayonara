/* GUI_AlternativeCovers.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * GUI_AlternativeCovers.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#include "GUI/AlternativeCovers/ui_GUI_AlternativeCovers.h"

#include "GUI_AlternativeCovers.h"
#include "AlternativeCoverItemDelegate.h"
#include "AlternativeCoverItemModel.h"

#include "Components/CoverLookup/CoverLookup.h"
#include "Components/CoverLookup/CoverLookupAlternative.h"

#include "Database/DatabaseConnector.h"

#include "GUI/Helper/Message/Message.h"
#include "Helper/Helper.h"
#include "Helper/MetaData/MetaData.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"

#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QRegExp>



GUI_AlternativeCovers::GUI_AlternativeCovers(QWidget* parent) :
	SayonaraDialog(parent),
	Ui::AlternativeCovers()
{

	setupUi(this);

	QString lib_path = _settings->get(Set::Lib_Path);

	if(QFile::exists(lib_path)){
		_last_path = lib_path;
	}

	else {
		_last_path = QDir::homePath();
	}

	_cl_alternative = 0;


	_cur_idx = -1;

    _model = new AlternateCoverItemModel(this);
    _delegate = new AlternateCoverItemDelegate(this);

	_is_searching = false;

	tv_images->setModel(_model);
	tv_images->setItemDelegate(_delegate);

	connect(btn_save, &QPushButton::clicked, this, &GUI_AlternativeCovers::save_button_pressed);
	connect(btn_cancel, &QPushButton::clicked, this, &GUI_AlternativeCovers::cancel_button_pressed);
	connect(btn_search, &QPushButton::clicked, this, &GUI_AlternativeCovers::search_button_pressed);
	connect(tv_images, &QTableView::pressed, this, &GUI_AlternativeCovers::cover_pressed);
	connect(btn_file, &QPushButton::clicked, this, &GUI_AlternativeCovers::open_file_dialog);
}


GUI_AlternativeCovers::~GUI_AlternativeCovers() {
	delete _model;
	delete _delegate;

	if(_cl_alternative) {
		_cl_alternative->stop();
	}

	delete_all_files();
}


void GUI_AlternativeCovers::language_changed() {
	retranslateUi(this);
}

void GUI_AlternativeCovers::connect_and_start() {

    reset_model();
	delete_all_files();

	connect(_cl_alternative, &CoverLookupAlternative::sig_cover_found, this, &GUI_AlternativeCovers::cl_new_cover);
	connect(_cl_alternative, &CoverLookupAlternative::sig_finished, this, &GUI_AlternativeCovers::cl_finished);

	_is_searching = true;

	btn_search->setText(tr("Stop"));
    btn_save->setEnabled(false);

	_cl_alternative->start();

	show();
}

void GUI_AlternativeCovers::start(int album_id, quint8 db_id, const CoverLocation& cl) {

    if(album_id < 0) return;

    Album album;
	LibraryDatabase* db = DB::getInstance(db_id);

    if( !db->getAlbumByID(album_id, album) ) return;

	start(album, cl);
}

void GUI_AlternativeCovers::start(const Album& album, const CoverLocation& cl) {

	le_search->setText(album.name + " " + ArtistList::get_major_artist(album.artists));

	if(!cl.valid){
		_cover_location = CoverLocation::get_cover_location(album);
	}
	else {
		_cover_location = cl;
	}

    _cl_alternative = new CoverLookupAlternative(this, album, 10);
    lab_info->setText(tr("for album") + " \"" + album.name + "\"");

	connect_and_start();
}


void GUI_AlternativeCovers::start(QString album_name, QString artist_name, const CoverLocation& cl) {

	le_search->setText(album_name + " " + artist_name);

	if(!cl.valid){
		_cover_location = CoverLocation::get_cover_location(album_name, artist_name);
	}

	else {
		_cover_location = cl;
	}

	_cl_alternative = new CoverLookupAlternative(this, album_name, artist_name, 10);
	lab_info->setText(tr("for album") + QString(" \"") + album_name + "\" " + tr("by") + " \"" + artist_name + "\"");

	connect_and_start();
}

void GUI_AlternativeCovers::start(const Artist& artist, const CoverLocation& cl) {

	le_search->setText(artist.name);

	if(!cl.valid){
		_cover_location = CoverLocation::get_cover_location(artist);
	}

	else {
		_cover_location = cl;
	}

    _cl_alternative = new CoverLookupAlternative(this, artist, 10);
    lab_info->setText(tr("for artist") + " \"" + artist.name + "\"");

	connect_and_start();

}


void GUI_AlternativeCovers::start(QString artist_name, const CoverLocation& cl) {

	le_search->setText(artist_name);
	if(!cl.valid){
		_cover_location = CoverLocation::get_cover_location(artist_name);
	}
	else {
		_cover_location = cl;
	}

	_cl_alternative = new CoverLookupAlternative(this, artist_name, 10);
    lab_info->setText(tr("for") + " \"" + artist_name + "\"");

	connect_and_start();
}


void GUI_AlternativeCovers::search_button_pressed() {

	if(_is_searching && _cl_alternative){
		_cl_alternative->stop();
		return;
	}

	_cl_alternative = new CoverLookupAlternative(this, le_search->text(), 10);

	connect_and_start();

}


void GUI_AlternativeCovers::save_button_pressed() {

	if(_cur_idx == -1) return;

	RowColumn rc = _model->cvt_2_row_col(_cur_idx);

	QModelIndex idx = _model->index(rc.row, rc.col);

	if(!idx.isValid()) {
		return;
	}

	QVariant var = _model->data(idx);
	CoverLocation cl = var.value<CoverLocation>();
	QFile file(cl.cover_path);

	if(!file.exists()) {
		Message::warning(tr("This cover does not exist"));
		return;
	}

	QImage img(cl.cover_path);
	if(img.isNull()){
		return;
	}

	img.save(_cover_location.cover_path);

	emit sig_cover_changed(_cover_location);
}


void GUI_AlternativeCovers::cancel_button_pressed() {

    hide();
    close();

	if(_cl_alternative) {
		_cl_alternative->stop();
	}

	delete_all_files();
}



void GUI_AlternativeCovers::cl_new_cover(const CoverLocation& cl) {

	QModelIndex model_idx;
	QVariant var;

	_filelist << cl;

    RowColumn rc = _model->cvt_2_row_col( _filelist.size() - 1 );
    RowColumn cur_idx_rc = _model->cvt_2_row_col( _cur_idx );
    btn_save->setEnabled( _model->is_valid(cur_idx_rc.row, cur_idx_rc.col) );

	model_idx = _model->index(rc.row, rc.col);
	var.setValue(cl);

	_model->setData(model_idx, var);
	lab_status->setText( tr("%1 covers found").arg(QString::number(_filelist.size())) ) ;
}


void GUI_AlternativeCovers::cl_finished(bool b) {

	Q_UNUSED(b)

	_is_searching = false;

	btn_search->setText(tr("Search"));
}



void GUI_AlternativeCovers::cover_pressed(const QModelIndex& idx) {

	_cur_idx = _model->cvt_2_idx(idx.row(), idx.column());

	bool valid = _model->is_valid(idx.row(), idx.column());

	btn_save->setEnabled(valid);

}


void GUI_AlternativeCovers::reset_model() {
    _model->removeRows(0, _model->rowCount());
	_model->insertRows(0, _model->rowCount());

	for(int y=0; y<_model->rowCount(); y++) {
		for(int x=0; x<_model->columnCount(); x++) {

            QModelIndex idx = _model->index(y,x);
			QString sayonara_logo = Helper::get_share_path() + "logo.png";
			_model->setData(idx, sayonara_logo);

        }
    }

    lab_status->setText("");
}




void GUI_AlternativeCovers::open_file_dialog() {

	QString lib_path = _settings->get(Set::Lib_Path);

	QDir dir( lib_path );

    QStringList filters;
        filters << "*.jpg";
        filters << "*.png";
        filters << "*.gif";

	dir.setFilter(QDir::Files);
	dir.setNameFilters(filters);


	for(const QString& f : dir.entryList()) {
		QFile::remove(dir.absoluteFilePath(f));
	}

    QStringList lst = QFileDialog::getOpenFileNames(this,
                                  tr("Open image files"),
                                  _last_path,
                                  filters.join(" "));

	if(lst.size() == 0) return;

    reset_model();

	int idx = 0;
	for(const QString& path : lst) {

		RowColumn rc = _model->cvt_2_row_col( idx );

		QModelIndex model_idx = _model->index(rc.row, rc.col);

		_model->setData(model_idx, path);

        idx ++;
    }
}


void GUI_AlternativeCovers::delete_all_files() {

	for(const CoverLocation& cl : _filelist) {
		if(!cl.valid) continue;
		QFile f(cl.cover_path);
		f.remove();
	}

	_filelist.clear();
}


void GUI_AlternativeCovers::closeEvent(QCloseEvent *e) {

	delete_all_files();

	SayonaraDialog::closeEvent(e);
}
