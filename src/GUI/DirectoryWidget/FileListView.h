/* FileListView.h */

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

#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include "GUI/Helper/SearchableWidget/SearchableView.h"
#include "GUI/Helper/Dragable/Dragable.h"

#include "Helper/Pimpl.h"

class FileListModel;
class MetaDataList;
class LibraryContextMenu;

class FileListView :
		public SearchableListView,
		private Dragable
{
	Q_OBJECT
	PIMPL(FileListView)

signals:
	void sig_info_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();

public:
	explicit FileListView(QWidget* parent=nullptr);
	virtual ~FileListView();

	QModelIndexList get_selected_rows() const;
	MetaDataList get_selected_metadata() const;
	QStringList get_selected_paths() const;

	void set_parent_directory(const QString& dir);

	QMimeData* get_mimedata() const override;
	int get_index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex get_model_index_by_index(int idx) const override;

private:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void init_context_menu();
};

#endif // FILELISTVIEW_H
