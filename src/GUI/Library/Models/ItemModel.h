/* ItemModel.h */

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

#ifndef LIBRARYITEMMODEL_H_
#define LIBRARYITEMMODEL_H_

#include "GUI/Utils/SearchableWidget/AbstractSearchModel.h"

#include "Utils/SetFwd.h"
#include "Utils/Pimpl.h"
#include "Utils/typedefs.h"

namespace Cover
{
    class Location;
}

class CustomMimeData;
class MetaDataList;
class AbstractLibrary;

namespace Library
{
	class ItemModel :
			public AbstractSearchTableModel
	{
		Q_OBJECT
		PIMPL(ItemModel)

	public:
		ItemModel(QObject* parent, AbstractLibrary* library);
		virtual ~ItemModel();

		/** Overloaded from QAbstractTableModel **/
		QVariant 	headerData ( int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const override;
		bool		setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;

		virtual int     columnCount(const QModelIndex& parent=QModelIndex()) const override;
		virtual bool 	insertColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
		virtual bool 	removeColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
		virtual bool	removeRows(int position, int rows, const QModelIndex& index=QModelIndex()) override;
		virtual bool	insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;
		int             last_row_count() const;

		/** AbstractSearchTableModel **/
		virtual bool has_items() const override;
		virtual QModelIndex getNextRowIndexOf(const QString& substr, int row, const QModelIndex& parent=QModelIndex()) override;
		virtual QModelIndex getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent=QModelIndex()) override;
		virtual QMap<QChar, QString> getExtraTriggers() override;

		virtual bool			is_selected(int id) const final;
		virtual const IndexSet& selections() const=0;

		virtual int				get_searchable_column() const=0;
		virtual QString			get_string(int row) const=0;
		virtual int				get_id_by_row(int row)=0;
        virtual Cover::Location	get_cover(const IndexSet& indexes) const=0;

		CustomMimeData*			get_mimedata() const;

		void                    refresh_data();

	protected:
		AbstractLibrary* library();
		const AbstractLibrary* library() const;
	};
}

#endif /* LIBRARYITEMMODEL_H_ */
