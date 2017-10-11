/* GUI_SomaFM.cpp */

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

/* GUI_SomaFM.cpp */

#include "3rdParty/SomaFM/ui/GUI_SomaFM.h"
#include "3rdParty/SomaFM/ui_GUI_SomaFM.h"
#include "3rdParty/SomaFM/ui/SomaFMStationModel.h"
#include "3rdParty/SomaFM/ui/SomaFMPlaylistModel.h"
#include "3rdParty/SomaFM/SomaFMLibrary.h"
#include "3rdParty/SomaFM/SomaFMStation.h"

#include "Utils/Utils.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"

#include "GUI/Utils/Delegates/StyledItemDelegate.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookup.h"

#include <QPixmap>
#include <QItemDelegate>

SomaFM::GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	Widget(parent)
{
	ui = new Ui::GUI_SomaFM();
	ui->setupUi(this);

	_library = new SomaFM::Library(this);

	SomaFM::StationModel* model_stations = new SomaFM::StationModel(this);

	ui->tv_stations->setModel(model_stations);
	ui->tv_stations->setSearchModel(model_stations);
	ui->tv_stations->setItemDelegate(new QItemDelegate(ui->tv_stations));
	ui->tv_stations->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tv_stations->setEnabled(false);
	ui->tv_stations->setColumnWidth(0, 20);

	ui->lv_playlists->setModel(new SomaFM::PlaylistModel());
	ui->lv_playlists->setItemDelegate(new Gui::StyledItemDelegate(ui->lv_playlists));
	ui->lv_playlists->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QPixmap logo = QPixmap(":/soma_icons/soma_logo.png")
		.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	ui->lab_image->setPixmap(logo);

	bool dark = (_settings->get(Set::Player_Style) == 1);
	QString description =
		"Listener-supported, commercial-free, underground/alternative radio<br /><br />" +
		Util::create_link("https://somafm.com", dark, "https://somafm.com");

	ui->lab_description->setText(description);
	ui->lab_donate->setText(Util::create_link("https://somafm.com/support/", dark));

	connect(_library, &SomaFM::Library::sig_stations_loaded, this, &SomaFM::GUI_SomaFM::stations_loaded);
	connect(_library, &SomaFM::Library::sig_station_changed, this, &SomaFM::GUI_SomaFM::station_changed);

	connect(ui->tv_stations, &QListView::activated, this, &SomaFM::GUI_SomaFM::station_index_changed);
	connect(ui->tv_stations, &QListView::clicked, this, &SomaFM::GUI_SomaFM::station_clicked);
	connect(ui->tv_stations, &QListView::doubleClicked, this, &SomaFM::GUI_SomaFM::station_double_clicked);

	connect(ui->lv_playlists, &QListView::doubleClicked, this, &SomaFM::GUI_SomaFM::playlist_double_clicked);
	connect(ui->lv_playlists, &QListView::activated, this, &SomaFM::GUI_SomaFM::playlist_double_clicked);

	_library->search_stations();
}

SomaFM::GUI_SomaFM::~GUI_SomaFM()
{
    if(_library) {
        _library->deleteLater(); _library = nullptr;
    }

	if(ui){
		delete ui; ui = nullptr;
	}
}

QFrame* SomaFM::GUI_SomaFM::header_frame() const
{
	return ui->header_frame;
}

void SomaFM::GUI_SomaFM::stations_loaded(const QList<SomaFM::Station>& stations)
{
    if(!ui){
        return;
    }

	sp_log(Log::Debug, this) << "Stations loaded";
	SomaFM::StationModel* model = static_cast<SomaFM::StationModel*>(ui->tv_stations->model());
	model->set_stations(stations);

	ui->tv_stations->setEnabled(true);
	ui->tv_stations->setDragEnabled(true);
	ui->tv_stations->setDragDropMode(QAbstractItemView::DragDrop);
}

void SomaFM::GUI_SomaFM::station_changed(const SomaFM::Station& station)
{
	SomaFM::StationModel* model = static_cast<SomaFM::StationModel*>(ui->tv_stations->model());
	model->replace_station(station);
}

void SomaFM::GUI_SomaFM::station_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_station(idx.row());
}

void SomaFM::GUI_SomaFM::selection_changed(const QModelIndexList& indexes)
{
	if(indexes.isEmpty()){
		return;
	}

	station_index_changed(indexes.first());
}


SomaFM::Station SomaFM::GUI_SomaFM::get_station(int row) const
{
	SomaFM::StationModel* station_model = static_cast<SomaFM::StationModel*>(ui->tv_stations->model());
	QModelIndex idx = station_model->index(row, 1);
	QString station_name = station_model->data(idx).toString();

	return _library->station(station_name);
}

void SomaFM::GUI_SomaFM::station_clicked(const QModelIndex &idx)
{
	if(!idx.isValid()){
		return;
	}

	SomaFM::StationModel* station_model = static_cast<SomaFM::StationModel*>(ui->tv_stations->model());
	if(!station_model->has_stations() && idx.column() == 0){
		station_model->set_waiting();
		_library->search_stations();

		return;
	}

	SomaFM::Station station = get_station(idx.row());

	if(idx.column() == 0){
		_library->set_station_loved( station.name(), !station.is_loved());
	}

	station_index_changed(idx);
}

void SomaFM::GUI_SomaFM::station_index_changed(const QModelIndex& idx)
{
	if(!idx.isValid()){
		return;
	}

	SomaFM::PlaylistModel* pl_model;
	SomaFM::Station station = get_station(idx.row());

	pl_model = static_cast<SomaFM::PlaylistModel*>(ui->lv_playlists->model());
	pl_model->setStation(station);

	ui->lab_description->setText(station.description());

    Cover::Lookup* cl = new Cover::Lookup(this);

    connect(cl, &Cover::Lookup::sig_cover_found, this, &SomaFM::GUI_SomaFM::cover_found);
	cl->fetch_cover(station.cover_location());
}


void SomaFM::GUI_SomaFM::playlist_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_playlist(idx.row());
}


void SomaFM::GUI_SomaFM::cover_found(const QString &cover_path)
{
    Cover::Lookup* cl = static_cast<Cover::Lookup*>(sender());
    if(Cover::Location::isInvalidLocation(cover_path)){
		return;
	}

	QPixmap pixmap = QPixmap(cover_path).scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if(pixmap.isNull()){
		pixmap = QPixmap(":/soma_icons/soma_logo.png").scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	ui->lab_image->setPixmap(pixmap);

	if(cl){
		cl->deleteLater();
	}
}


