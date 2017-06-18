/* LibraryItemModelArtists.h */

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
 * LibraryItemModelArtistts.h
 *
 *  Created on: Apr 26, 2011
 *      Author: Lucio Carreras
 */

#ifndef LIBRARYITEMMODELARTISTS_H_
#define LIBRARYITEMMODELARTISTS_H_

#include "LibraryItemModel.h"
#include "Helper/MetaData/MetaDataFwd.h"
#include "Helper/Pimpl.h"

class LibraryItemModelArtists :
		public LibraryItemModel
{
	Q_OBJECT
	PIMPL(LibraryItemModelArtists)

	public:
		LibraryItemModelArtists(QObject* parent=nullptr);
		virtual ~LibraryItemModelArtists();

		QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

		Qt::ItemFlags flags(const QModelIndex &index) const override;

		bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;
		bool setData(const QModelIndex &index, const ArtistList& artists, int role=Qt::DisplayRole);

		int get_id_by_row(int row) override;
		QString get_string(int row) const override;

		CoverLocation get_cover(const SP::Set<int>& indexes) const override;
		int get_searchable_column() const override;
};

#endif /* LIBRARYITEMMODELARTISTS_H_ */
