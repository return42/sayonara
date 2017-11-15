/* DirectoryTreeView.h */

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

#ifndef DIRECTORYTREEVIEW_H
#define DIRECTORYTREEVIEW_H

#include "GUI/Utils/SearchableWidget/SearchableView.h"
#include "GUI/Utils/Widgets/WidgetTemplate.h"
#include "GUI/Utils/Dragable/Dragable.h"
#include "Utils/Pimpl.h"

#include <QTreeView>
#include <QModelIndexList>

class LibraryContextMenu;
class DirectoryModel;
class MetaDataList;
class IconProvider;

namespace Library
{
	class Info;
}

class DirectoryTreeView :
		public Gui::WidgetTemplate<SearchableTreeView>,
		protected Dragable
{
	Q_OBJECT
	PIMPL(DirectoryTreeView)

signals:
	void sig_info_clicked();
	void sig_edit_clicked();
	void sig_lyrics_clicked();
	void sig_delete_clicked();
	void sig_play_next_clicked();
	void sig_append_clicked();
	void sig_directory_loaded(const QModelIndex& index);
	void sig_enter_pressed();

public:
	explicit DirectoryTreeView(QWidget* parent=nullptr);
	virtual ~DirectoryTreeView();

	QModelIndex		search(const QString& search_term);
	QString			directory_name(const QModelIndex& index);

	QModelIndexList	selected_items() const;
	MetaDataList	selected_metadata() const;
	QStringList		selected_paths() const;

	QMimeData*		get_mimedata() const override;

private:
	void init_context_menu();

private slots:
	void directory_loaded(const QString& dir_name);

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

	// SayonaraSelectionView
	int index_by_model_index(const QModelIndex& idx) const override;
	QModelIndex model_index_by_index(int idx) const override;

	void select_match(const QString& str, SearchDirection direction) override;

	void skin_changed() override;
};

#endif // DIRECTORYTREEVIEW_H
