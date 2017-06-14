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

#include "Helper/Helper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include "GUI/Helper/Delegates/ListDelegate.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/Covers/CoverLookup.h"

#include <QPixmap>
#include <QItemDelegate>

GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	SayonaraWidget(parent)
{
	ui = new Ui::GUI_SomaFM();
	ui->setupUi(this);

	_library = new SomaFMLibrary(this);

	SomaFMStationModel* model_stations = new SomaFMStationModel(this);

	ui->tv_stations->setModel(model_stations);
	ui->tv_stations->setSearchModel(model_stations);
	ui->tv_stations->setItemDelegate(new QItemDelegate(ui->tv_stations));
	ui->tv_stations->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tv_stations->setEnabled(false);
	ui->tv_stations->setColumnWidth(0, 20);

	ui->lv_playlists->setModel(new SomaFMPlaylistModel());
	ui->lv_playlists->setItemDelegate(new ListDelegate(ui->lv_playlists));
	ui->lv_playlists->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QPixmap logo = QPixmap(":/soma_icons/soma_logo.png")
		.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	ui->lab_image->setPixmap(logo);

	bool dark = (_settings->get(Set::Player_Style) == 1);
	QString description =
		"Listener-supported, commercial-free, underground/alternative radio<br /><br />" +
		Helper::create_link("https://somafm.com", dark, "https://somafm.com");

	ui->lab_description->setText(description);
	ui->lab_donate->setText(Helper::create_link("https://somafm.com/support/", dark));

	connect(_library, &SomaFMLibrary::sig_stations_loaded, this, &GUI_SomaFM::stations_loaded);
	connect(_library, &SomaFMLibrary::sig_station_changed, this, &GUI_SomaFM::station_changed);

	connect(ui->tv_stations, &QListView::activated, this, &GUI_SomaFM::station_index_changed);
	connect(ui->tv_stations, &QListView::clicked, this, &GUI_SomaFM::station_clicked);
	connect(ui->tv_stations, &QListView::doubleClicked, this, &GUI_SomaFM::station_double_clicked);

	connect(ui->lv_playlists, &QListView::doubleClicked, this, &GUI_SomaFM::playlist_double_clicked);
	connect(ui->lv_playlists, &QListView::activated, this, &GUI_SomaFM::playlist_double_clicked);

	_library->search_stations();
}

GUI_SomaFM::~GUI_SomaFM()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}

QFrame* GUI_SomaFM::header_frame() const
{
	return ui->header_frame;
}

void GUI_SomaFM::stations_loaded(const QList<SomaFMStation>& stations)
{
	sp_log(Log::Debug, this) << "Stations loaded";
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(ui->tv_stations->model());
	model->set_stations(stations);

	ui->tv_stations->setEnabled(true);
	ui->tv_stations->setDragEnabled(true);
	ui->tv_stations->setDragDropMode(QAbstractItemView::DragDrop);
}

void GUI_SomaFM::station_changed(const SomaFMStation& station)
{
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(ui->tv_stations->model());
	model->replace_station(station);
}

void GUI_SomaFM::station_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_station(idx.row());
}

void GUI_SomaFM::selection_changed(const QModelIndexList& indexes){
	if(indexes.isEmpty()){
		return;
	}

	station_index_changed(indexes.first());
}


SomaFMStation GUI_SomaFM::get_station(int row) const
{
	SomaFMStationModel* station_model = static_cast<SomaFMStationModel*>(ui->tv_stations->model());
	QModelIndex idx = station_model->index(row, 1);
	QString station_name = station_model->data(idx).toString();

	return _library->get_station(station_name);
}

void GUI_SomaFM::station_clicked(const QModelIndex &idx){
	if(!idx.isValid()){
		return;
	}

	SomaFMStationModel* station_model = static_cast<SomaFMStationModel*>(ui->tv_stations->model());
	if(!station_model->has_stations() && idx.column() == 0){
		station_model->set_waiting();
		_library->search_stations();

		return;
	}

	SomaFMStation station = get_station(idx.row());

	if(idx.column() == 0){
		_library->set_station_loved( station.get_name(), !station.is_loved());
	}

	station_index_changed(idx);
}

void GUI_SomaFM::station_index_changed(const QModelIndex& idx){
	if(!idx.isValid()){
		return;
	}

	SomaFMPlaylistModel* pl_model;
	SomaFMStation station = get_station(idx.row());

	pl_model = static_cast<SomaFMPlaylistModel*>(ui->lv_playlists->model());
	pl_model->setStation(station);

	ui->lab_description->setText(station.get_description());

	CoverLookup* cl = new CoverLookup(this);

	connect(cl, &CoverLookup::sig_cover_found, this, &GUI_SomaFM::cover_found);
	cl->fetch_cover(station.get_cover_location());
}


void GUI_SomaFM::playlist_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_playlist(idx.row());
}


void GUI_SomaFM::cover_found(const QString &cover_path){
	CoverLookup* cl = static_cast<CoverLookup*>(sender());
	if(CoverLocation::isInvalidLocation(cover_path)){
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


