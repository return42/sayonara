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

	enum class Status {
		Waiting,
		Error,
		OK
	};

	Status _status;

	// QAbstractItemModel interface
public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	int columnCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;


	// AbstractSearchModelInterface interface
public:
	QModelIndex getFirstRowIndexOf(QString substr) override;
	QModelIndex getNextRowIndexOf(QString substr, int cur_row, const QModelIndex& parent) override;
	QModelIndex getPrevRowIndexOf(QString substr, int cur_row, const QModelIndex& parent) override;
	QMap<QChar, QString> getExtraTriggers() override;

	void set_stations(const QList<SomaFMStation>& stations);
	void replace_station(const SomaFMStation& station);
	bool has_stations() const;
	void set_waiting();
};


#endif // SomaFMStationModel_H
