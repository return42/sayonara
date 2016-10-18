/* LibraryItemModelArtists.cpp */

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
 * LibraryItemModelArtists.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryItemModelArtists.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Library/Helper/ColumnHeader.h"
#include "GUI/Library/Helper/ColumnIndex.h"

#include "GUI/Helper/GUI_Helper.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Library/SearchMode.h"
#include "Helper/MetaData/Artist.h"

#include <QAbstractListModel>
#include <QStringList>
#include <QPixmap>

struct LibraryItemModelArtists::Private
{
	ArtistList	artists;
	QPixmap		pm_multi;
	QPixmap		pm_single;
};

LibraryItemModelArtists::LibraryItemModelArtists() :
	LibraryItemModel()
{
	_m = Pimpl::make<LibraryItemModelArtists::Private>();

	_m->pm_single = GUI::get_pixmap("play", QSize(16, 16));
	_m->pm_multi = GUI::get_pixmap("sampler", QSize(16, 16));
}

LibraryItemModelArtists::~LibraryItemModelArtists() {}

int LibraryItemModelArtists::get_id_by_row(int row)
{
	if(row < 0 || row >= _m->artists.size()){
		return -1;
	}

	else {
		return _m->artists[row].id;
	}
}

QString LibraryItemModelArtists::get_string(int row) const
{
	if(row < 0 || row >= _m->artists.size()){
		return QString();
	}

	else {
		return _m->artists[row].name;
	}
}


QVariant LibraryItemModelArtists::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _m->artists.size())
		return QVariant();

	int row = index.row();
	int col = index.column();

	ColumnIndex::Artist idx_col = (ColumnIndex::Artist) col;
	const Artist& artist = _m->artists[row];


	if(role == Qt::TextAlignmentRole){

		switch(idx_col) {
			case ColumnIndex::Artist::Name:
				return (int) (Qt::AlignLeft | Qt::AlignVCenter);
			default:
				return (int) (Qt::AlignRight | Qt::AlignVCenter);
		}
	}

	else if(role == Qt::DecorationRole){
		if(idx_col == ColumnIndex::Artist::NumAlbums){
			if(artist.num_albums > 1){
				return _m->pm_multi;
			}
			return _m->pm_single;
		}
	}

	else if(role == Qt::DisplayRole) {

		switch(idx_col) {
			case ColumnIndex::Artist::Name:
				return artist.name;

			case ColumnIndex::Artist::Tracks:
				return QString::number(artist.num_songs) + " " + tr("tracks");

			default:
				return QVariant();
		}
	}

	return QVariant();
}


bool LibraryItemModelArtists::setData(const QModelIndex& index, const QVariant& value, int role)
{

	if (index.isValid() && role == Qt::DisplayRole) {

		int row = index.row();

		Artist::fromVariant(value,  _m->artists[row]);

		emit dataChanged(index, this->index(row, _header_names.size() - 1));

		return true;
	}

	return false;
}

bool LibraryItemModelArtists::setData(const QModelIndex& index, const ArtistList& artists, int role)
{

	if (index.isValid() && role == Qt::DisplayRole) {

		int row = index.row();

		_m->artists = artists;

		emit dataChanged(index, this->index(row + artists.size() - 1, _header_names.size() - 1));

		return true;
	}

	return false;
}



Qt::ItemFlags LibraryItemModelArtists::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}

QModelIndex LibraryItemModelArtists::getFirstRowIndexOf(const QString& substr) {
	if(_m->artists.isEmpty()) {
		return this->index(-1, -1);
	}

	else{
		return getNextRowIndexOf(substr, 0);
	}

}


QModelIndex	LibraryItemModelArtists::getNextRowIndexOf(const QString& substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	Settings* settings = Settings::getInstance();
	Library::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	QString coverted_string = Library::convert_search_string(substr, mask);

	int len = _m->artists.size();
	if( len == 0 ) return this->index(-1, -1);

	for(int i=0; i<len; i++) {
		int row_idx = (i + row) % len;

		QString artist_name = _m->artists[row_idx].name;
		artist_name = Library::convert_search_string(artist_name, mask);

		if(artist_name.contains(coverted_string))
		{
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}


QModelIndex	LibraryItemModelArtists::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent) {

	Q_UNUSED(parent)

	Settings* settings = Settings::getInstance();
	Library::SearchModeMask mask = settings->get(Set::Lib_SearchMode);
	QString coverted_string = Library::convert_search_string(substr, mask);

	int len = _m->artists.size();
	if( len < row) row = len - 1;

	for(int i=0; i<len; i++) {

		if(row - i < 0) {
			row = len - 1;
		}

		int row_idx = (row-i) % len;

		QString artist_name = _m->artists[row_idx].name;
		artist_name = Library::convert_search_string(artist_name, mask);

		if(artist_name.contains(coverted_string))
		{
			return this->index(row_idx, 0);
		}
	}

	return this->index(-1, -1);
}


CoverLocation LibraryItemModelArtists::get_cover(const SP::Set<int>& indexes) const
{
	if(indexes.isEmpty() || indexes.size() > 1){
		return CoverLocation();
	}

	int idx = indexes.first();
	if(idx < 0 || idx > _m->artists.size()){
		return CoverLocation();
	}

	const Artist& artist = _m->artists[idx];
	return CoverLocation::get_cover_location(artist);
}
