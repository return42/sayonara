#include "ui/GUI_SomaFM.h"
#include "ui/SomaFMStationModel.h"

#include "Components/CoverLookup/CoverLookup.h"
#include "GUI/Helper/Delegates/ListDelegate.h"
#include "GUI/Helper/GUI_Helper.h"
#include "Helper/Helper.h"


#include <QStringListModel>
#include <QPixmap>

GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_SomaFM()

{
	setupUi(this);

	_library = new SomaFMLibrary(this);

	SomaFMStationModel* model_stations = new SomaFMStationModel(this);

	tv_stations->setModel(model_stations);
	tv_stations->setAbstractModel(model_stations);
//	tv_stations->setItemDelegate(new ListDelegate(tv_stations));
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

	connect(tv_stations, &QListView::activated, this, &GUI_SomaFM::station_index_changed);
//	connect(tv_stations, &SearchableTableView::sig_selection_changed, this, &GUI_SomaFM::selection_changed);

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

void GUI_SomaFM::selection_changed(const QModelIndexList& indexes){

	if(indexes.isEmpty()){
		return;
	}

	station_index_changed(indexes[0]);
}


void GUI_SomaFM::station_index_changed(const QModelIndex& idx){

	if(!idx.isValid()){
		return;
	}

	QStringListModel* pl_model = static_cast<QStringListModel*>(lv_playlists->model());

	QString station_name = tv_stations->model()->data(idx, Qt::WhatsThisRole).toString();

	SomaFMStation station = _library->get_station(station_name);


	QStringList urls = station.get_urls();
	QStringList texts;

	for(QString& url : urls){
		SomaFMStation::UrlType type = station.get_url_type(url);
		if(type == SomaFMStation::UrlType::MP3){
			texts << station_name + " (mp3)";		
		}

		else if(type == SomaFMStation::UrlType::AAC){
			texts << station_name + " (aac)";
		}

		else{
			texts << url;
		}
	}

	pl_model->setStringList(texts);

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


