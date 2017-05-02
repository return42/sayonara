/* LibraryItemModel.h */

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

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"

#include "Helper/SetFwd.h"
#include "Helper/Pimpl.h"

class CoverLocation;
class CustomMimeData;
class MetaDataList;

class LibraryItemModel :
		public AbstractSearchTableModel
{
	Q_OBJECT
	PIMPL(LibraryItemModel)

public:

	LibraryItemModel();
	virtual ~LibraryItemModel();

	/** Overloaded from QAbstractTableModel **/
	QVariant 	headerData ( int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const override;
	bool		setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;

	virtual int			rowCount(const QModelIndex &parent=QModelIndex()) const override;
	virtual int 		columnCount(const QModelIndex& parent=QModelIndex()) const override;
	virtual bool 		insertColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
	virtual bool 		removeColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
	virtual bool		removeRows(int position, int rows, const QModelIndex& index=QModelIndex()) override;
	virtual bool		insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;

	virtual QModelIndex	getFirstRowIndexOf(const QString& substr) override;
	virtual QModelIndex getNextRowIndexOf(const QString& substr, int row, const QModelIndex& parent=QModelIndex()) override;
	virtual QModelIndex getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent=QModelIndex()) override;

	virtual void			add_selections(const SP::Set<int>& rows) final;
	virtual bool			is_selected(int id) const final;
	virtual bool			has_selections() const final;
	virtual void			clear_selections() final;

	virtual int				get_searchable_column() const=0;
	virtual QString			get_string(int row) const=0;
	virtual int				get_id_by_row(int row)=0;
	virtual CoverLocation	get_cover(const SP::Set<int>& indexes) const=0;


	virtual QMap<QChar, QString> getExtraTriggers() override;

	void set_mimedata(const MetaDataList& v_md);

	CustomMimeData* get_mimedata() const;
};

#endif /* LIBRARYITEMMODEL_H_ */
