#include "ui/GUI_SomaFM.h"
#include "ui/SomaFMStationModel.h"

#include "Helper/Helper.h"
#include "GUI/Helper/GUI_Helper.h"
#include "GUI/Helper/Delegates/ListDelegate.h"
#include "Components/CoverLookup/CoverLookup.h"

#include <QStringListModel>
#include <QPixmap>
#include <QItemDelegate>

GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_SomaFM()

{
	setupUi(this);

	_library = new SomaFMLibrary(this);

	SomaFMStationModel* model_stations = new SomaFMStationModel(this);

	tv_stations->setModel(model_stations);
	tv_stations->setAbstractModel(model_stations);
	tv_stations->setItemDelegate(new QItemDelegate(tv_stations));
	tv_stations->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tv_stations->setEnabled(false);
	tv_stations->setColumnWidth(0, 20);

	lv_playlists->setModel(new QStringListModel());
	lv_playlists->setItemDelegate(new ListDelegate(lv_playlists));
	lv_playlists->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QPixmap logo = QPixmap(":/soma_icons/soma_logo.png")
		.scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

	lab_image->setPixmap(logo);

	QString description = 
		"Listener-supported, commercial-free, underground/alternative radio<br /><br />" +
		Helper::create_link("https://somafm.com", this->is_dark(), "https://somafm.com");

	lab_description->setText(description);
	lab_donate->setText(Helper::create_link("https://somafm.com/support/", is_dark()));

	connect(_library, &SomaFMLibrary::sig_stations_loaded, this, &GUI_SomaFM::stations_loaded);
	connect(_library, &SomaFMLibrary::sig_station_changed, this, &GUI_SomaFM::station_changed);

	connect(tv_stations, &QListView::activated, this, &GUI_SomaFM::station_index_changed);
	connect(tv_stations, &QListView::clicked, this, &GUI_SomaFM::station_clicked);
	connect(tv_stations, &SearchableTableView::sig_selection_changed, this, &GUI_SomaFM::selection_changed);

	connect(lv_playlists, &QListView::doubleClicked, this, &GUI_SomaFM::playlist_double_clicked);
	connect(lv_playlists, &QListView::activated, this, &GUI_SomaFM::playlist_double_clicked);

	_library->search_stations();
}

GUI_SomaFM::~GUI_SomaFM()
{

}

QComboBox* GUI_SomaFM::get_libchooser() const
{
	return combo_lib_chooser;
}

void GUI_SomaFM::stations_loaded(const QList<SomaFMStation>& stations)
{
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(tv_stations->model());
	model->set_stations(stations);
	tv_stations->setEnabled(true);
}

void GUI_SomaFM::station_changed(const SomaFMStation& station)
{
	SomaFMStationModel* model = static_cast<SomaFMStationModel*>(tv_stations->model());
	model->replace_station(station);
}

void GUI_SomaFM::selection_changed(const QModelIndexList& indexes){

	if(indexes.isEmpty()){
		return;
	}

	station_index_changed(indexes[0]);
}


SomaFMStation GUI_SomaFM::get_station(int row) const
{
	SomaFMStationModel* station_model = static_cast<SomaFMStationModel*>(tv_stations->model());
	QModelIndex idx = station_model->index(row, 1);
	QString station_name = station_model->data(idx).toString();

	return _library->get_station(station_name);
}

void GUI_SomaFM::station_clicked(const QModelIndex &idx){
	if(!idx.isValid()){
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

	QStringListModel* pl_model;
	SomaFMStation station = get_station(idx.row());

	QStringList entries;
	QStringList urls = station.get_urls();

	for(QString& url : urls){
		SomaFMStation::UrlType type = station.get_url_type(url);
		if(type == SomaFMStation::UrlType::MP3){
			entries << station.get_name() + " (mp3)";
		}

		else if(type == SomaFMStation::UrlType::AAC){
			entries << station.get_name() + " (aac)";
		}

		else{
			entries << url;
		}
	}

	pl_model = static_cast<QStringListModel*>(lv_playlists->model());
	pl_model->setStringList(entries);

	lab_description->setText(station.get_description());

	CoverLookup* cl = new CoverLookup(this);

	connect(cl, &CoverLookup::sig_cover_found, this, &GUI_SomaFM::cover_found);
	cl->fetch_cover(station.get_cover_location());
}


void GUI_SomaFM::playlist_double_clicked(const QModelIndex& idx)
{
	_library->create_playlist_from_playlist(idx.row());
}


void GUI_SomaFM::cover_found(const CoverLocation &cover_location){

	CoverLookup* cl = static_cast<CoverLookup*>(sender());
	if(!cover_location.valid){
		return;
	}

	QPixmap pixmap = QPixmap(cover_location.cover_path).scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	if(pixmap.isNull()){
		pixmap = QPixmap(":/soma_icons/soma_logo.png").scaled(QSize(200, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	lab_image->setPixmap(pixmap);

	if(cl){
		cl->deleteLater();
	}
}


