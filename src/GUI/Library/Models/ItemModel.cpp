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

#include "ItemModel.h"
#include "Components/Library/AbstractLibrary.h"
#include "GUI/Library/Utils/ColumnHeader.h"
#include "GUI/Utils/CustomMimeData.h"
#include "Utils/MetaData/MetaDataList.h"

#include "Utils/globals.h"
#include "Utils/Set.h"
#include "Utils/Logger/Logger.h"

#include <algorithm>
#include <QUrl>

using namespace Library;

struct ItemModel::Private
{
	AbstractLibrary*	library=nullptr;
	QStringList			header_names;
	int                 old_row_count;

	Private(AbstractLibrary* library) :
		library(library),
		old_row_count(0)
	{}
};


ItemModel::ItemModel(QObject* parent, AbstractLibrary* library) :
	AbstractSearchTableModel(parent)
{
	m = Pimpl::make<ItemModel::Private>(library);
}

ItemModel::~ItemModel() {}

QVariant ItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
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

bool ItemModel::set_header_data(const QStringList& names)
{
   m->header_names = names;
   emit headerDataChanged(Qt::Horizontal, 0, names.size());
   return true;
}


int ItemModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return m->header_names.size();
}

bool ItemModel::removeRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)

	beginRemoveRows(QModelIndex(), row, row + count - 1);
	m->old_row_count -= count;
	endRemoveRows();

	return true;
}

bool ItemModel::insertRows(int row, int count, const QModelIndex& index)
{
	Q_UNUSED(index)

	beginInsertRows(QModelIndex(), row, row + count - 1);
	m->old_row_count += count;
	endInsertRows();

	return true;
}

int ItemModel::last_row_count() const
{
	return m->old_row_count;
}


QMap<QChar, QString> ItemModel::getExtraTriggers()
{
	return QMap<QChar, QString>();
}

CustomMimeData* ItemModel::get_mimedata() const
{
	CustomMimeData* mimedata = new CustomMimeData();
	QList<QUrl> urls;

	const MetaDataList& track_mimedata = m->library->current_tracks();

	if(track_mimedata.isEmpty()){
		sp_log(Log::Warning, this) << this->objectName() << " does not have any mimedata";
		mimedata->setText("No tracks");
	}

	else
	{
		for(const MetaData& md : track_mimedata){
			QUrl url(QString("file://") + md.filepath());
			urls << url;
		}

		mimedata->set_metadata(track_mimedata);
		mimedata->setText("tracks");
		mimedata->setUrls(urls);
	}

	return mimedata;
}

void ItemModel::refresh_data()
{
	emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}


bool ItemModel::is_selected(int id) const
{
	return selections().contains(id);
}

bool ItemModel::has_items() const
{
	return (rowCount() > 0);
}

QModelIndex ItemModel::getNextRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
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
		title = Library::Util::convert_search_string(title, search_mode());

		if(title.contains(substr)) {
			return this->index(row_idx, get_searchable_column());
		}
	}

	return QModelIndex();
}


QModelIndex ItemModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
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
		title = Library::Util::convert_search_string(title, search_mode());

		if(title.contains(substr)) {
			return this->index(row_idx, get_searchable_column());
		}
	}

	return QModelIndex();
}


AbstractLibrary* ItemModel::library()
{
	return m->library;
}

const AbstractLibrary *ItemModel::library() const
{
	return m->library;
}
