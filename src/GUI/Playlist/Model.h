/* PlaylistItemModel.h */

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
 * PlaylistItemModel.h
 *
 *  Created on: Apr 8, 2011
 *      Author: Lucio Carreras
 */

#ifndef PLAYLISTITEMMODEL_H_
#define PLAYLISTITEMMODEL_H_

#include "GUI/Utils/SearchableWidget/SearchableModel.h"
#include "Utils/typedefs.h"
#include "Utils/Playlist/PlaylistFwd.h"
#include "Utils/SetFwd.h"
#include "Utils/Pimpl.h"

#include <QString>

class MetaData;
class MetaDataList;
class CustomMimeData;

class PlaylistItemModel :
		public SearchableListModel
{
	Q_OBJECT
	PIMPL(PlaylistItemModel)

public:
	explicit PlaylistItemModel(PlaylistPtr pl, QObject* parent=nullptr);
	virtual ~PlaylistItemModel();

	int rowCount(const QModelIndex &parent=QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	void clear();

	void remove_rows(const IndexSet& rows);
	void move_rows(const IndexSet& rows, int target_index);
	IndexSet move_rows_up(const IndexSet& rows);
	IndexSet move_rows_down(const IndexSet& rows);
	void copy_rows(const IndexSet& rows, int target_index);

	void set_current_track(int row);
	int current_track() const;

	QModelIndex getPrevRowIndexOf(const QString& substr, int row, const QModelIndex &parent=QModelIndex()) override;
	QModelIndex getNextRowIndexOf(const QString& substr, int row, const QModelIndex &parent=QModelIndex()) override;
	QMap<QChar, QString> getExtraTriggers() override;

	const MetaData& metadata(int row) const;
	MetaDataList metadata(const IndexSet& rows) const;

	CustomMimeData* custom_mimedata(const QModelIndexList& indexes) const;
	QMimeData* mimeData(const QModelIndexList& indexes) const override;

	bool has_local_media(const IndexSet& rows) const;


private slots:
	void				playlist_changed(int pl_idx);
};

#endif /* PLAYLISTITEMMODEL_H_ */
