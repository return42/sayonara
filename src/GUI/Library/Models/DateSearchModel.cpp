/* DateSearchModel.cpp */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DateSearchModel.h"
#include "Helper/Library/DateFilter.h"

#include "Helper/Settings/Settings.h"
#include <QList>

struct DateSearchModel::Private
{
	QList<Library::DateFilter> date_filters;
	Settings* settings=nullptr;

	Private()
	{
		settings = Settings::getInstance();
		date_filters = settings->get(Set::Lib_DateFilters);

		for(int i=date_filters.size() - 1; i>=0; i--){
			if(!date_filters[i].valid()){
				date_filters.removeAt(i);
			}
		}
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
	Q_UNUSED(parent)
	return _m->date_filters.size();
}

QVariant DateSearchModel::data(const QModelIndex& index, int role) const
{
	if(role == Qt::DisplayRole){
		return _m->date_filters[index.row()].name();
	}

	return QVariant();
}

void DateSearchModel::set_data(const Library::DateFilter filter, int idx)
{
	if(idx < 0 || idx >= _m->date_filters.size()){
		return;
	}

	_m->date_filters[idx] = filter;
	_m->settings->set(Set::Lib_DateFilters, _m->date_filters);

	emit dataChanged(index(idx, 0), index(idx, 1));
}

void DateSearchModel::add_data(const Library::DateFilter filter)
{
	_m->date_filters << filter;
	_m->settings->set(Set::Lib_DateFilters, _m->date_filters);

	emit dataChanged(index(rowCount() - 1, 0),
					 index(rowCount() - 1, 0));
}

void DateSearchModel::remove(int idx)
{
	if(idx < 0 || idx >= _m->date_filters.size()){
		return;
	}

	_m->date_filters.removeAt(idx);
	_m->settings->set(Set::Lib_DateFilters, _m->date_filters);

	emit dataChanged(index(idx, 0),
					 index(rowCount() - 1, 0));
}


QModelIndex DateSearchModel::getFirstRowIndexOf(const QString& substr)
{
	return getNextRowIndexOf(substr, 0, QModelIndex());
}

QModelIndex DateSearchModel::getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int rows = rowCount();
	if(rows == 0){
		return QModelIndex();
	}

	for(int i=0; i<rows; i++){
		int row = (i + cur_row) % rows;
		QString str = _m->date_filters[row].name();
		str = Library::convert_search_string(str, search_mode());
		if(str.contains(substr)){
			return this->index(row, 0);
		}
	}

	return QModelIndex();
}

QModelIndex DateSearchModel::getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int rows = rowCount();
	if(rows == 0){
		return QModelIndex();
	}

	for(int i=0; i<rows; i++){
		int row = (cur_row - i);
		if(row < 0){
			row = rows - 1;
		}

		QString str = _m->date_filters[row].name();
		str = Library::convert_search_string(str, search_mode());
		if(str.contains(substr)){
			return this->index(row, 0);
		}
	}

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
