/* FileListModel.h */

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

#ifndef FILE_LIST_MODEL_H
#define FILE_LIST_MODEL_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"

#include <QStringList>
#include <QModelIndex>

class QVariant;

class FileListModel :
	public AbstractSearchListModel
{
	Q_OBJECT

	public:
		explicit FileListModel(QObject* parent=nullptr);
		virtual ~FileListModel();

		void set_parent_directory(const QString& dir);
		QStringList get_files() const;

        bool        has_items() const override;
		QModelIndex getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
		QModelIndex getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
		QMap<QChar, QString> getExtraTriggers() override;

		QVariant data(const QModelIndex &index, int role) const override;

		int rowCount(const QModelIndex &parent = QModelIndex()) const override;

		QMimeData* mimeData(const QModelIndexList &indexes) const override;
		Qt::ItemFlags flags(const QModelIndex& index) const override;

	private:
		QStringList _files;
};

#endif
