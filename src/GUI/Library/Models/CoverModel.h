/* CoverModel.h */

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

#ifndef ALBUMCOVERMODEL_H
#define ALBUMCOVERMODEL_H

#include "GUI/Library/Models/ItemModel.h"
#include "Utils/Pimpl.h"
#include "Utils/typedefs.h"

class Album;
class AlbumList;

class QSize;

namespace Cover
{
    class Location;
    class Lookup;
}

namespace Library
{
	class CoverModel :
			public ItemModel
	{
		Q_OBJECT
		PIMPL(CoverModel)

		public:
			explicit CoverModel(QObject* parent, AbstractLibrary* library);
			virtual ~CoverModel();

			void set_data(const AlbumList& albums);

			// QAbstractItemModel interface
		public:
			int         rowCount(const QModelIndex& parent=QModelIndex()) const override;
			int         columnCount(const QModelIndex& paren=QModelIndex()) const override;
			QVariant    data(const QModelIndex& index, int role) const override;
			QSize       item_size() const;
			const IndexSet& selections() const override;

			bool            has_items() const override;
			QModelIndex     getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
			QModelIndex     getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
			QMap<QChar, QString> getExtraTriggers() override;

			int             get_searchable_column() const override;
			QString         get_string(int idx) const override;
			int             get_id_by_row(int idx) override;
            Cover::Location get_cover(const IndexSet& indexes) const override;


			int             zoom() const;
			void            set_max_columns(int columns);

		public slots:
			void            set_zoom(int zoom, const QSize& view_size);

		private slots:
			void            next_hash();
	};
}

#endif // ALBUMCOVERMODEL_H