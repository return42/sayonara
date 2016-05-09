#include "GUI_SomaFM.h"
#include "SomaFMLibrary.h"
#include "SomaFMLinkListModel.h"

#include "Components/CoverLookup/CoverLookup.h"


#include <QStringListModel>
#include <QPixmap>

GUI_SomaFM::GUI_SomaFM(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_SomaFM()

{
	setupUi(this);
	_library = new SomaFMLibrary(this);

	QStringListModel* model_stations = new QStringListModel();
	SomaFMLinkListModel* model_links = new SomaFMLinkListModel();

	lv_stations->setModel(model_stations);
	lv_links->setModel(model_links);

	connect(_library, &SomaFMLibrary::sig_stations_loaded, this, &GUI_SomaFM::stations_loaded);
	connect(_library, &SomaFMLibrary::sig_station_loaded, this, &GUI_SomaFM::station_loaded);
	connect(lv_stations, &QListView::clicked, this, &GUI_SomaFM::station_index_changed);

	_library->init_stations();
}

GUI_SomaFM::~GUI_SomaFM()
{

}

QComboBox* GUI_SomaFM::get_libchooser() const
{
	return combo_lib_chooser;
}

void GUI_SomaFM::stations_loaded(const QStringList& stations)
{

	QStringListModel* model = static_cast<QStringListModel*>(lv_stations->model());
	model->setStringList(stations);
}

void GUI_SomaFM::station_loaded(const SomaFMStation& station)
{
	SomaFMLinkListModel* model = static_cast<SomaFMLinkListModel*>(lv_links->model());

	model->set_metadata(station.get_metadata());

	lab_description->setText(station.get_description());

	CoverLookup* cl = new CoverLookup(this);

	connect(cl, &CoverLookup::sig_cover_found, this, &GUI_SomaFM::cover_found);
	cl->fetch_cover(station.get_cover_location());

}

void GUI_SomaFM::station_index_changed(const QModelIndex& idx){

	if(!idx.isValid()){
		return;
	}

	QString data = lv_stations->model()->data(idx).toString();
	_library->request_station(data);

}

void GUI_SomaFM::cover_found(const CoverLocation &cover_location){

	CoverLookup* cl = static_cast<CoverLookup*>(sender());
	if(!cover_location.valid){
		return;
	}

	QPixmap pixmap(cover_location.cover_path);
	lab_image->setPixmap(pixmap);

	if(cl){
		cl->deleteLater();
	}
}


SomaFMLibraryContainer::SomaFMLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{
	Q_INIT_RESOURCE(SomaFMIcons);
}

QString SomaFMLibraryContainer::get_name() const
{
	return "SomaFM";
}

QString SomaFMLibraryContainer::get_display_name() const
{
	return "SomaFM";
}

QIcon SomaFMLibraryContainer::get_icon() const
{
	return QIcon(":/soma_icons/soma.png");
}

QWidget* SomaFMLibraryContainer::get_ui() const
{
	return ui;
}

QComboBox* SomaFMLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

QMenu* SomaFMLibraryContainer::get_menu()
{
	return nullptr;
}

void SomaFMLibraryContainer::init_ui()
{
	ui = new GUI_SomaFM(nullptr);
}

