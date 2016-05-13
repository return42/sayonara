#include "SomaFMStationModel.h"

SomaFMStationModel::SomaFMStationModel(QObject *parent) :
	AbstractSearchListModel(parent)
{
}


int SomaFMStationModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return _stations.size();
}

QVariant SomaFMStationModel::data(const QModelIndex& index, int role) const
{
	Q_UNUSED(role)

	if(!index.isValid()){
		return QVariant();
	}

	return _stations[index.row()];

}

QModelIndex SomaFMStationModel::getFirstRowIndexOf(QString substr)
{
	return getNextRowIndexOf(substr, 0, QModelIndex());
}

QModelIndex SomaFMStationModel::getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row + 1; i<_stations.size(); i++){
		if(_stations[i].contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();

}

QModelIndex SomaFMStationModel::getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)
	for(int i=cur_row - 1; i>=0; i--){
		if(_stations[i].contains(substr, Qt::CaseInsensitive)){
			return this->index(i, 0);
		}
	}

	return QModelIndex();
}

QMap<QChar, QString> SomaFMStationModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}

void SomaFMStationModel::setStringList(const QStringList& list)
{
	this->removeRows(0, this->rowCount());
	this->insertRows(0, list.size());

	beginInsertRows(QModelIndex(), 0, list.size() - 1);
	_stations = list;
	endInsertRows();

	emit dataChanged(this->index(0), this->index(list.size() - 1));
}
