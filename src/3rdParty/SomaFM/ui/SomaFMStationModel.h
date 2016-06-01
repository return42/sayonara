#ifndef SomaFMStationModel_H
#define SomaFMStationModel_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "3rdParty/SomaFM/SomaFMStation.h"
#include <QMap>
#include <QList>

class SomaFMStationModel : public AbstractSearchTableModel
{
	Q_OBJECT
public:
	explicit SomaFMStationModel(QObject *parent = 0);

private:
	QList<SomaFMStation> _stations;

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const;
	int columnCount(const QModelIndex& parent=QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role) const;

	// AbstractSearchModelInterface interface
public:
	QModelIndex getFirstRowIndexOf(QString substr);
	QModelIndex getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent);
	QModelIndex getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent);
	QMap<QChar, QString> getExtraTriggers();


	void set_stations(const QList<SomaFMStation>& stations);
};

#endif // SomaFMStationModel_H
