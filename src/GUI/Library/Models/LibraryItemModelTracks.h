/* LibraryItemModelTracks.h */

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
 * LibraryItemModelTracks.h
 *
 *  Created on: Apr 24, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMMODELTRACKS_H_
#define LIBRARYITEMMODELTRACKS_H_

#include "GUI/Library/Models/LibraryItemModel.h"
#include "Helper/Pimpl.h"

class MetaDataList;
class LibraryItemModelTracks :
		public LibraryItemModel
{
	Q_OBJECT

public:
    LibraryItemModelTracks(QObject* parent, AbstractLibrary* library);
	virtual ~LibraryItemModelTracks();

    /** AbstractSearchTableModel **/
    Qt::ItemFlags   flags(const QModelIndex &index) const override;
    QVariant        data(const QModelIndex &index, int role) const override;
    bool            setData(const QModelIndex &index, const QVariant &value, int role) override;
    int             rowCount(const QModelIndex &parent) const;

    /** LibraryItemModel.h **/
    CoverLocation   get_cover(const IndexSet& indexes) const override;
    int             get_searchable_column() const override;
    int             get_id_by_row(int row) override;
    QString         get_string(int row) const override;
    const IndexSet& selections() const override;
};

#endif /* LIBRARYITEMMODELTRACKS_H_ */
