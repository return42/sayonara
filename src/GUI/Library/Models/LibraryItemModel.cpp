/* LibraryItemModel.cpp */

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

#include "LibraryItemModel.h"
#include "Components/Library/AbstractLibrary.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Helper/CustomMimeData.h"
#include "Helper/MetaData/MetaDataList.h"

#include "Helper/globals.h"
#include "Helper/Set.h"
#include "Helper/Logger/Logger.h"

#include <algorithm>
#include <QUrl>

struct LibraryItemModel::Private
{
    AbstractLibrary* library=nullptr;
	QStringList		header_names;
	MetaDataList	track_mimedata;
	SP::Set<int>	selections;

	int				n_rows;
	int				n_cols;

    Private(AbstractLibrary* library) :
        library(library),
        n_rows(0),
        n_cols(0)
    {}
};


LibraryItemModel::LibraryItemModel(QObject* parent, AbstractLibrary* library) :
	AbstractSearchTableModel(parent)
{
    m = Pimpl::make<LibraryItemModel::Private>(library);
}

LibraryItemModel::~LibraryItemModel() {}

QVariant LibraryItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if (role != Qt::DisplayRole){
		return QVariant();
	}

	if(!between(section, m->header_names)){
		return QVariant();
	}

	if (orientation == Qt::Horizontal){
		return m->header_names[section];
	}

	return QVariant();
}


bool LibraryItemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	Q_UNUSED(role)

	if(!between(section, m->header_names)){
		return false;
	}

	if(orientation == Qt::Horizontal){
		m->header_names[section] = value.toString();
		emit headerDataChanged(orientation, section, section);
	}

	return true;
}


int LibraryItemModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return m->n_rows;
}


int LibraryItemModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return m->header_names.size();
}


bool LibraryItemModel::insertColumns(int position, int cols, const QModelIndex &index)
{
	Q_UNUSED(index)

	beginInsertColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++) {
		m->header_names.insert(i, "");
	}

	endInsertColumns();
	return true;
}


bool LibraryItemModel::removeColumns(int position, int cols, const QModelIndex &index)
{
	Q_UNUSED(index)

	beginRemoveColumns(QModelIndex(), position, position+cols-1);

	for(int i=position; i<position+cols; i++) {
		m->header_names.removeAt(position);
	}

	endRemoveColumns();
	return true;
}

bool LibraryItemModel::removeRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)

	beginRemoveRows(QModelIndex(), row, row + count - 1);

	for(int i=row; i<row + count; i++){
		m->selections.remove( get_id_by_row(i) );
	}

	m->n_rows -= count;

	endRemoveRows();

	return true;
}

bool LibraryItemModel::insertRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)

	beginInsertRows(QModelIndex(), row, row + count - 1);
	m->n_rows += count;
	endInsertRows();

	return true;
}


QMap<QChar, QString> LibraryItemModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}


void LibraryItemModel::set_mimedata(const MetaDataList& v_md)
{
	m->track_mimedata = v_md;
}


CustomMimeData* LibraryItemModel::get_mimedata() const
{
	CustomMimeData* mimedata = new CustomMimeData();
	QList<QUrl> urls;

	if(m->track_mimedata.isEmpty()){
		sp_log(Log::Warning) << this->objectName() << " does not have any mimedata. Do you forget to call LibraryItemModel::set_mimedata first?";
		mimedata->setText("No tracks");
	}

	else{
		for(const MetaData& md : m->track_mimedata){
			QUrl url(QString("file://") + md.filepath());
			urls << url;
		}

		mimedata->setMetaData(m->track_mimedata);
		mimedata->setText("tracks");
		mimedata->setUrls(urls);
	}

    return mimedata;
}

void LibraryItemModel::refresh_data()
{
    emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}


bool LibraryItemModel::has_selections() const
{
	return !(m->selections.isEmpty());
}


void LibraryItemModel::add_selections(const SP::Set<int>& rows)
{
	std::for_each(rows.begin(), rows.end(), [=](int row){
		m->selections.insert(get_id_by_row(row));
	});
}


bool LibraryItemModel::is_selected(int id) const
{
	return m->selections.contains(id);
}


void LibraryItemModel::clear_selections()
{
	m->selections.clear();
}


QModelIndex	LibraryItemModel::getFirstRowIndexOf(const QString& substr)
{
	if(rowCount() == 0) {
		return QModelIndex();
	}

	return getNextRowIndexOf(substr, 0);
}


QModelIndex LibraryItemModel::getNextRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int len = rowCount();
	if(len == 0) {
		return QModelIndex();
	}

	for(int i=0; i<len; i++)
	{
		int row_idx = (i + row) % len;

		QString title = get_string(row_idx);
		title = Library::convert_search_string(title, search_mode());

		if(title.contains(substr)) {
			return this->index(row_idx, get_searchable_column());
		}
	}

	return QModelIndex();
}


QModelIndex LibraryItemModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	Q_UNUSED(parent)

	int len = rowCount();
	if(len < row) row = len - 1;

	for(int i=0; i<len; i++)
	{
		if(row - i < 0) {
			row = len - 1;
		}

		int row_idx = (row - i) % len;

		QString title = get_string(row_idx);
		title = Library::convert_search_string(title, search_mode());

		if(title.contains(substr)) {
			return this->index(row_idx, get_searchable_column());
		}
	}

	return QModelIndex();
}


AbstractLibrary* LibraryItemModel::library()
{
    return m->library;
}

const AbstractLibrary *LibraryItemModel::library() const
{
    return m->library;
}
