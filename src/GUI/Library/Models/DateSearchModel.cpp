#include "DateSearchModel.h"
#include "Helper/Library/DateFilter.h"
#include <QList>

struct DateSearchModel::Private
{
    QList<Library::DateFilter> date_filters;

    Private()
    {
	Library::DateFilter df1("Last 1 month");
	df1.set_newer_than(Library::DateFilter::TimeSpan::Months, 1);
	Library::DateFilter df2("Last 3 months");
	df2.set_newer_than(Library::DateFilter::TimeSpan::Months, 3);
	Library::DateFilter df3("Last 6 months");
	df3.set_newer_than(Library::DateFilter::TimeSpan::Months, 6);
	Library::DateFilter df4("Last Year");
	df4.set_newer_than(Library::DateFilter::TimeSpan::Years, 1);
	Library::DateFilter df5("Between 1 and 3 months");
	df5.set_between(Library::DateFilter::TimeSpan::Months, 1, Library::DateFilter::TimeSpan::Months, 3);
	Library::DateFilter df6("Older than 1 year");
	df6.set_older_than(Library::DateFilter::TimeSpan::Years, 1);
	Library::DateFilter df7("Older than 2 years");
	df7.set_older_than(Library::DateFilter::TimeSpan::Years, 2);

	date_filters << df1 << df2 << df3 << df4 << df5 << df6 << df7;
    }
};

DateSearchModel::DateSearchModel(QObject* parent) :
    AbstractSearchListModel(parent)
{
    _m = Pimpl::make<DateSearchModel::Private>();

    emit dataChanged(index(0, 0), index(rowCount()-1, 0));
}

DateSearchModel::~DateSearchModel(){}


int DateSearchModel::rowCount(const QModelIndex& parent) const
{
    return _m->date_filters.size();
}

QVariant DateSearchModel::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DisplayRole){
	return _m->date_filters[index.row()].name();
    }

    return QVariant();
}

QModelIndex DateSearchModel::getFirstRowIndexOf(const QString& substr)
{
    return getNextRowIndexOf(substr, 0, QModelIndex());
}

QModelIndex DateSearchModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
    return QModelIndex();
}

QModelIndex DateSearchModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
    return QModelIndex();
}

QMap<QChar, QString> DateSearchModel::getExtraTriggers()
{
    return QMap<QChar, QString>();
}

Library::DateFilter DateSearchModel::get_filter(int row) const
{
    if(row < 0 || row >= rowCount()){
	return Library::DateFilter("");
    }

    return _m->date_filters[row];
}
