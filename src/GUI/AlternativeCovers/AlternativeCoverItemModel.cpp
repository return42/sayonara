/* AlternateCoverItemModel.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * AlternateCoverItemModel.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: luke
 */

#include "GUI/AlternativeCovers/AlternativeCoverItemModel.h"
#include "Components/CoverLookup/CoverLocation.h"
#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include "Helper/Logger/Logger.h"


AlternateCoverItemModel::AlternateCoverItemModel(QObject* parent) : QAbstractTableModel(parent) {

	_pathlist.reserve(10);
}

AlternateCoverItemModel::~AlternateCoverItemModel() {

}

RowColumn AlternateCoverItemModel::cvt_2_row_col(int idx) const {


	RowColumn p;

    if(idx < 0) {
        p.row = -1;
        p.col = -1;
        p.valid = false;
    }

	p.row = idx / columnCount();
	p.col = idx % columnCount();
    p.valid = true;

	return p;
}

int AlternateCoverItemModel::cvt_2_idx(int row, int col) const {
    if(row < 0 || col < 0) return -1;

	return row * columnCount() + col;
}


int AlternateCoverItemModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 2;

}
int AlternateCoverItemModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 5;
}

QVariant AlternateCoverItemModel::data(const QModelIndex &index, int role) const
{

	int lin_idx = this->cvt_2_idx(index.row(), index.column());
    if(lin_idx < 0) return QVariant();

	/*sp_log(Log::Debug) << "I want to have a cover: " << index <<
				" valid? " << index.isValid() <<
				" pathlist size=" << _pathlist.size() <<
				"Lin idx: " << lin_idx;*/

     if (!index.isValid() || _pathlist.size() <= lin_idx) {
         return QVariant();
     }


	 if(role == Qt::DisplayRole) {

		 QVariant var;
		 var.setValue<CoverLocation>(_pathlist[lin_idx]);
		 //sp_log(Log::Debug) << "Try to conver Cover location to " << _pathlist[lin_idx].cover_path;
		 return var;
	 }

	 else
		 return QVariant();
}

Qt::ItemFlags AlternateCoverItemModel::flags(const QModelIndex &index) const{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);

}

bool AlternateCoverItemModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if (!index.isValid())
		 return false;


	int lin_idx = cvt_2_idx(index.row(), index.column());

    if(lin_idx >= _pathlist.size() || lin_idx < 0)
        return false;

	 if(role == Qt::DisplayRole) {

		 if(value.canConvert<CoverLocation>()){
			_pathlist[lin_idx] = value.value<CoverLocation>();
		 }

		 emit dataChanged(index, index);
		 return true;
	 }

	 else
		 return false;

}

bool AlternateCoverItemModel::insertRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	_pathlist.clear();
	CoverLocation cl = CoverLocation::getInvalidLocation();

	for(int i=0; i<rows; i++) {
		for(int j=0; j<columnCount(); j++) {
			_pathlist << cl;
		}
	}

	endInsertRows();
	return true;

}
bool AlternateCoverItemModel::removeRows(int position, int rows, const QModelIndex &index) {
	Q_UNUSED(index);

	 beginRemoveRows(QModelIndex(), position, position+rows-1);

	 _pathlist.clear();

	 endRemoveRows();
	 return true;

}

bool AlternateCoverItemModel::is_valid(int row, int col){

    int idx = cvt_2_idx(row, col);
    if(idx < 0) return false;

    if( ! _pathlist[ idx ].valid ){
		return false;
	}

	return true;
}
