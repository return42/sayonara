#ifndef SomaFMStationModel_H
#define SomaFMStationModel_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "3rdParty/SomaFM/SomaFMStation.h"
#include <QMap>
#include <QList>
class QMimeData;
class SomaFMStationModel : public AbstractSearchTableModel
{
	Q_OBJECT
public:
	explicit SomaFMStationModel(QObject *parent = 0);

private:
	QList<SomaFMStation> _stations;

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	// AbstractSearchModelInterface interface
public:
	QModelIndex getFirstRowIndexOf(QString substr);
	QModelIndex getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent);
	QModelIndex getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent);
	QMap<QChar, QString> getExtraTriggers();


	void set_stations(const QList<SomaFMStation>& stations);
	void replace_station(const SomaFMStation& station);
};


#endif // SomaFMStationModel_H
