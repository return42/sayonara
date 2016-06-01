#include "SomaFMStationModel.h"
#include "GUI/Helper/GUI_Helper.h"

SomaFMStationModel::SomaFMStationModel(QObject *parent) :
	AbstractSearchTableModel(parent)
{
}


int SomaFMStationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)

	if(_stations.isEmpty()){
		return 1;
	}

	return _stations.size();
}

int SomaFMStationModel::columnCount(const QModelIndex& parent) const
{
	return 2;
}

QVariant SomaFMStationModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	int col = index.column();


	if(!index.isValid()){
		sp_log(Log::Debug) << "Index not valid";
		return QVariant();
	}

	if(role == Qt::TextAlignmentRole) {
		return (int)(Qt::AlignVCenter| Qt::AlignLeft);
	}

	if(row < 0 || row >= rowCount()) {
		return QVariant();
	}

	if(role == Qt::SizeHintRole) {
		if(col == 0){
			return QSize(20, 20);
		}
	}

	if(role == Qt::DecorationRole) {
		if(_stations.isEmpty()){
			return QVariant();
		}

		if(col == 1){
			return QVariant();
		}

		SomaFMStation station = _stations[row];
		if(station.is_loved()){
			return GUI::get_icon("star.png");
		}
	}

	if(role == Qt::DisplayRole && col == 1) {
		if(_stations.isEmpty()){
			return tr("Initializing") + "...";
		}

		SomaFMStation station = _stations[row];
		return station.get_station_name();
	}

	if(role == Qt::WhatsThisRole) {
		if(_stations.isEmpty()){
			return "";
		}

		SomaFMStation station = _stations[row];
		return station.get_station_name();
	}


	return QVariant();
}



QModelIndex SomaFMStationModel::getFirstRowIndexOf(QString substr)
{
	return getNextRowIndexOf(substr, 0, QModelIndex());
}

QModelIndex SomaFMStationModel::getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row + 1; i<_stations.size(); i++){

		QString name = _stations[i].get_station_name();
		QString desc = _stations[i].get_description();

		QString str = name + desc;

		if(str.contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();

}

QModelIndex SomaFMStationModel::getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row - 1; i>=0; i--){

		QString name = _stations[i].get_station_name();
		QString desc = _stations[i].get_description();

		QString str = name + desc;

		if(str.contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();
}

QMap<QChar, QString> SomaFMStationModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}

void SomaFMStationModel::set_stations(const QList<SomaFMStation>& stations)
{
	int n_stations = stations.size();

	this->removeRows(0, this->rowCount());
	this->insertRows(0, n_stations);


	beginInsertRows(QModelIndex(), 0, n_stations - 1);
	_stations = stations;
	endInsertRows();

	emit dataChanged(this->index(0, 0), this->index(n_stations - 1, 1));
}
