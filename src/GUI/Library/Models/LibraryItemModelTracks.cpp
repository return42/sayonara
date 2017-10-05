/* LibraryItemModelTracks.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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

#include "Components/Library/AbstractLibrary.h"
#include "Components/Covers/CoverLocation.h"

#include "GUI/Library/Helper/ColumnIndex.h"

#include "Helper/globals.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Language.h"
#include "Helper/Set.h"

LibraryItemModelTracks::LibraryItemModelTracks(QObject* parent, AbstractLibrary* library) :
    LibraryItemModel(parent, library)
{}

LibraryItemModelTracks::~LibraryItemModelTracks() {}

QVariant LibraryItemModelTracks::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int col = index.column();

	if (!index.isValid()) {
		return QVariant();
	}

    const MetaDataList& tracks = library()->tracks();

    if (row >= tracks.count()) {
		return QVariant();
	}

	ColumnIndex::Track idx_col = (ColumnIndex::Track) col;

	if (role == Qt::TextAlignmentRole)
	{
		int alignment = Qt::AlignVCenter;

		if (idx_col == ColumnIndex::Track::TrackNumber ||
			idx_col == ColumnIndex::Track::Bitrate ||
			idx_col == ColumnIndex::Track::Length ||
			idx_col == ColumnIndex::Track::Year ||
			idx_col == ColumnIndex::Track::Filesize)
		{
			alignment |= Qt::AlignRight;
		}

		else {
			alignment |= Qt::AlignLeft;
		}

		return alignment;
	}

	else if (role == Qt::DisplayRole || role==Qt::EditRole)
	{
        const MetaData& md = tracks[row];

		switch(idx_col)
		{
			case ColumnIndex::Track::TrackNumber:
				return QVariant( md.track_num );

			case ColumnIndex::Track::Title:
				return QVariant( md.title );

			case ColumnIndex::Track::Artist:
				return QVariant( md.artist() );

			case ColumnIndex::Track::Length:
				return QVariant( Helper::cvt_ms_to_string(md.length_ms) );

			case ColumnIndex::Track::Album:
				return QVariant(md.album());

			case ColumnIndex::Track::Year:
				if(md.year == 0){
					return Lang::get(Lang::None);
				}
				
				return md.year;

			case ColumnIndex::Track::Bitrate:
				return QString::number(md.bitrate / 1000) + " kbit/s";

			case ColumnIndex::Track::Filesize:
				return Helper::File::calc_filesize_str(md.filesize);

			case ColumnIndex::Track::Rating:
				if(role == Qt::DisplayRole) {
					return QVariant();
				}

				return QVariant(md.rating);

			default:
				return QVariant();
		}
	}

	return QVariant();
}


Qt::ItemFlags LibraryItemModelTracks::flags(const QModelIndex &index = QModelIndex()) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	if(index.column() == (int) ColumnIndex::Track::Rating) {
		return (QAbstractItemModel::flags(index) | Qt::ItemIsEditable);
	}

	return QAbstractItemModel::flags(index);
}

bool LibraryItemModelTracks::setData(const QModelIndex &index, const QVariant &value, int role) 
{
	if(!index.isValid()){
		return false;
	}

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
		int row = index.row();
		int col = index.column();

        if(col == (int) ColumnIndex::Track::Rating)
        {
            library()->change_track_rating(row, value.toInt());
            emit dataChanged(index, this->index(row, columnCount() - 1));
            return true;
		}
	}

    return false;
}

int LibraryItemModelTracks::rowCount(const QModelIndex&) const
{
    return library()->tracks().count();
}


int LibraryItemModelTracks::get_id_by_row(int row)
{
    const MetaDataList& tracks = library()->tracks();

    if(!between(row, tracks)){
		return -1;
	}

	else {
        return tracks[row].id;
	}
}

QString LibraryItemModelTracks::get_string(int row) const
{
    const MetaDataList& tracks = library()->tracks();

    if(!between(row, tracks)){
        return QString();
    }

    else {
        return tracks[row].title;
    }
}


CoverLocation LibraryItemModelTracks::get_cover(const IndexSet& indexes) const
{
	if(indexes.isEmpty()){
		return CoverLocation();
	}

    const MetaDataList& tracks = library()->tracks();
    SP::Set<AlbumID> album_ids;

    for(int idx : indexes)
    {
        if(!between(idx, tracks)){
            continue;
        }

        album_ids.insert( tracks[idx].album_id );
        if(album_ids.size() > 1) {
			return CoverLocation();
		}
	}

    return CoverLocation::get_cover_location( tracks.first() );
}


int LibraryItemModelTracks::get_searchable_column() const
{
    return (int) ColumnIndex::Track::Title;
}


const IndexSet& LibraryItemModelTracks::selections() const
{
    return library()->selected_tracks();
}
