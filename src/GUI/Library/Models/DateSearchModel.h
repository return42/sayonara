#ifndef DATESEARCHMODEL_H
#define DATESEARCHMODEL_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"

#include "Helper/Pimpl.h"

namespace Library {class DateFilter;}

class DateSearchModel :
	public AbstractSearchListModel
{
    Q_OBJECT

public:
	DateSearchModel(QObject* parent=nullptr);
	~DateSearchModel();

public:
	int rowCount(const QModelIndex& parent=QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;

	void set_data(const Library::DateFilter filter, int idx);
	void add_data(const Library::DateFilter filter);
	void remove(int idx);

public:
	QModelIndex getFirstRowIndexOf(const QString& substr) override;
	QModelIndex getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	QModelIndex getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
	QMap<QChar, QString> getExtraTriggers();

	Library::DateFilter get_filter(int row) const;

private:
	PIMPL(DateSearchModel)
};

#endif // DATESEARCHMODEL_H
