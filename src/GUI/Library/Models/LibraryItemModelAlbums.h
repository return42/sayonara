/* LibraryItemModelAlbums.h */

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
 * LibraryItemModelAlbums.h
 *
 *  Created on: Apr 26, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMMODELALBUMS_H_
#define LIBRARYITEMMODELALBUMS_H_

#include "GUI/Library/Models/LibraryItemModel.h"
#include "Helper/Pimpl.h"

#include <QPixmap>

class AlbumList;
class ColumnHeader;
class LibraryItemModelAlbums : public LibraryItemModel {

Q_OBJECT

public:
	LibraryItemModelAlbums();
	virtual ~LibraryItemModelAlbums();

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::DisplayRole) override;
	bool setData(const QModelIndex& index, const AlbumList& albums, int role=Qt::DisplayRole);

	void sort(int column, Qt::SortOrder order) override;

	QModelIndex	getFirstRowIndexOf(QString substr) override;
	QModelIndex	getNextRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;
	QModelIndex	getPrevRowIndexOf(QString substr, int row, const QModelIndex& parent=QModelIndex()) override;

	int	get_id_by_row(int row) override;
	QString get_string(int row) const override;

	CoverLocation get_cover(const SP::Set<int>& indexes) const override;


private:
	PIMPL(LibraryItemModelAlbums)

};

#endif /* LIBRARYITEMMODELALBUMS_H_ */
