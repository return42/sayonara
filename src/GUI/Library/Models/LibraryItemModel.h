/* LibraryItemModel.h */

/* Copyright (C) 2011-2016  Lucio Carreras
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



#define COL_ALBUM_MACROS
#define COL_ALBUM_SAMPLER 0
#define COL_ALBUM_NAME 1
#define COL_ALBUM_DURATION 2
#define COL_ALBUM_N_SONGS 3
#define COL_ALBUM_YEAR 4
#define COL_ALBUM_RATING 5

#define COL_ARTIST_MACROS
#define COL_ARTIST_N_ALBUMS 0
#define COL_ARTIST_NAME 1
#define COL_ARTIST_TRACKS 2

#define COL_TRACK_NUM 0
#define COL_TITLE 1
#define COL_ARTIST 2
#define COL_ALBUM 3
//#define COL_DISCNUMBER 4
#define COL_YEAR 4
#define COL_LENGTH 5
#define COL_BITRATE 6
#define COL_FILESIZE 7
#define COL_TRACK_RATING 8

#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Library/ColumnHeader.h"
#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "Helper/Set.h"

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QHash>




class LibraryItemModel : public AbstractSearchTableModel{

	Q_OBJECT

public:

	LibraryItemModel(QList<ColumnHeader>& headers);
	virtual ~LibraryItemModel();

	QStringList 	get_header_names() const;
	int 			get_n_cols() const;
	int 			calc_shown_col(int col) const;
	bool			is_col_shown(int col) const;



	/** Overloaded from QAbstractTableModel **/
	QVariant 	headerData ( int section, Qt::Orientation orientation, int role=Qt::DisplayRole ) const override;
	int			rowCount(const QModelIndex &parent=QModelIndex()) const override;
	int 		columnCount(const QModelIndex& parent=QModelIndex()) const override;
	bool 		insertColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
	bool 		removeColumns(int position, int cols, const QModelIndex &index=QModelIndex()) override;
	bool		removeRows(int position, int rows, const QModelIndex& index=QModelIndex()) override;
	bool		insertRows(int row, int count, const QModelIndex &parent=QModelIndex()) override;

	virtual void			add_selections(const SP::Set<int>& rows) final;
	virtual void			add_selection(int row) final;
	virtual bool			is_selected(int id) final;
	virtual bool			has_selections() final;
	virtual void			remove_selection(int row) final;
	virtual void			clear_selections() final ;

	virtual QMap<QChar, QString> getExtraTriggers();

	void set_new_header_names(QStringList& lst);
	void set_mimedata(const MetaDataList& v_md);

	CustomMimeData* get_mimedata();


protected:
	QStringList		_header_names;
	QVector<bool>	_cols_active;
	MetaDataList	_md_mimedata;
	IDList			_selections;

	int				_n_rows;
	int				_n_all_cols;

protected:
	virtual int		get_id_by_row(int row)=0;

};



#endif /* LIBRARYITEMMODEL_H_ */
