/* LibraryItemModelTracks.cpp */

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
 * LibraryItemModelTracks.cpp
	 *
 *  Created on: Apr 24, 2011
 *      Author: Lucio Carreras
 */


#include "LibraryItemModelTracks.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Helper/ColumnIndex.h"

#include "Helper/globals.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Library/SearchMode.h"


LibraryItemModelTracks::LibraryItemModelTracks() :
	LibraryItemModel()
{

}


LibraryItemModelTracks::~LibraryItemModelTracks() {

}


QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const{

	int row = index.row();
	int col = index.column();

	if (!index.isValid())
		return QVariant();

	if (row >= _tracks.size())
		return QVariant();

	ColumnIndex::Track idx_col = (ColumnIndex::Track) col;

	if( role == Qt::TextAlignmentRole ){
		int alignment = Qt::AlignVCenter;
		switch(idx_col)
		{
			case ColumnIndex::Track::Title:
			case ColumnIndex::Track::Album:
			case ColumnIndex::Track::Artist:
				alignment |= Qt::AlignLeft;
				break;
			default:
				alignment |= Qt::AlignRight;
		}

		return alignment;
	}

	else if (role == Qt::DisplayRole || role==Qt::EditRole) {

		const MetaData& md = _tracks.at(row);

		switch(idx_col) {
			case ColumnIndex::Track::TrackNumber:
				return QVariant( md.track_num );

			case ColumnIndex::Track::Title:
				return QVariant( md.title );

			case ColumnIndex::Track::Artist:
				return QVariant( md.artist );

			case ColumnIndex::Track::Length:
				return QVariant( Helper::cvt_ms_to_string(md.length_ms) );

			case ColumnIndex::Track::Album:
				return QVariant(md.album);

			case ColumnIndex::Track::Year:
				if(md.year == 0){
					return tr("None");
				}
				
				return md.year;

			case ColumnIndex::Track::Bitrate:
				return QString::number(md.bitrate / 1000) + " kbit/s";

			case ColumnIndex::Track::Filesize:
				return Helper::File::calc_filesize_str(md.filesize);

			case ColumnIndex::Track::Rating:
				return QVariant(md.rating);
			default:
				return QVariant();
		}
	}

	else if (role == Qt::TextAlignmentRole) {

		if (idx_col == ColumnIndex::Track::TrackNumber ||
			idx_col == ColumnIndex::Track::Bitrate ||
			idx_col == ColumnIndex::Track::Length ||
			idx_col == ColumnIndex::Track::Year ||
			idx_col == ColumnIndex::Track::Filesize)
		{
			return Qt::AlignRight + Qt::AlignVCenter;
		}

		else return Qt::AlignLeft + Qt::AlignVCenter;
	}


	return QVariant();
}


Qt::ItemFlags LibraryItemModelTracks::flags(const QModelIndex &index = QModelIndex()) const{

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if(index.column() == (int) ColumnIndex::Track::Rating) {

		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	}

	return QAbstractItemModel::flags(index);
}

bool LibraryItemModelTracks::setData(const QModelIndex &index, const QVariant &value, int role) {

	if(!index.isValid()){
		return false;
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {

		int row = index.row();
		int col = index.column();

		if(col == (int) ColumnIndex::Track::Rating) {
			_tracks[row].rating = value.toInt();
		}

		else{

			if(!MetaData::fromVariant(value, _tracks[row])) {
				return false;
			}
		}

		emit dataChanged(index, this->index(row, _header_names.size() - 1));

		return true;
	}

	return false;
}

bool LibraryItemModelTracks::setData(const QModelIndex&index, const MetaDataList&v_md, int role)
{
	if(!index.isValid()){
		return false;
	}

	if (role == Qt::EditRole || role == Qt::DisplayRole) {

		int row = index.row();

		_tracks = v_md;

		emit dataChanged(index, this->index(row + v_md.size() - 1, _header_names.size() - 1));

		return true;
	}

	return false;
}


int LibraryItemModelTracks::get_id_by_row(int row)
{
	if(!between(row, _tracks)){
		return -1;
	}

	else {
		return _tracks[row].id;
	}
}

QString LibraryItemModelTracks::get_string(int row) const
{
	if(!between(row, _tracks)){
		return QString();
	}

	else {
		return _tracks[row].title;
	}
}



QModelIndex	LibraryItemModelTracks::getFirstRowIndexOf(QString substr) {
	if(_tracks.isEmpty()) {
		return this->index(-1, -1);
	}

	return getNextRowIndexOf(substr, 0);
}

QModelIndex LibraryItemModelTracks::getNextRowIndexOf(QString substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	int len = _tracks.size();
	if(_tracks.isEmpty()) {
		return this->index(-1, -1);
	}

	Settings* settings = Settings::getInstance();
	Library::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	substr = Library::convert_search_string(substr, mask);

	for(int i=0; i< len; i++)
	{
		int row_idx = (i + row) % len;

		QString title = _tracks[row_idx].title;
		title = Library::convert_search_string(title, mask);

		if(title.contains(substr)) {
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}

QModelIndex LibraryItemModelTracks::getPrevRowIndexOf(QString substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	Settings* settings = Settings::getInstance();
	Library::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	substr = Library::convert_search_string(substr, mask);

	int len = _tracks.size();
	if(len < row) row = len - 1;
	for(int i=0; i< len; i++)
	{
		if(row - i < 0) {
			row = len - 1;
		}

		int row_idx = (row - i) % len;

		QString title = _tracks[row_idx].title;
		title = Library::convert_search_string(title, mask);

		if(title.contains(substr))
		{
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}

