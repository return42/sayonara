/* FileListView.cpp */

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

#include "FileListView.h"
#include "FileListModel.h"

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"

#include "Helper/globals.h"
#include "DirectoryIconProvider.h"
#include "DirectoryDelegate.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/Settings/Settings.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QPainter>
#include <QMimeData>
#include <QApplication>

struct FileListView::Private
{
	LibraryContextMenu*	context_menu=nullptr;
	FileListModel*		model=nullptr;
};

FileListView::FileListView(QWidget* parent) :
	SearchableListView(parent),
	Dragable(this)
{
	m = Pimpl::make<Private>();
	m->model = new FileListModel(this);

	this->setModel(m->model);
	this->setSearchModel(m->model);
	this->setItemDelegate(new DirectoryDelegate(this));
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setDragEnabled(true);
	this->setIconSize(QSize(16, 16));
}

FileListView::~FileListView() {}

void FileListView::mousePressEvent(QMouseEvent* event)
{
	SearchableListView::mousePressEvent(event);

	if(event->button() & Qt::RightButton){
		QPoint pos = QWidget::mapToGlobal(event->pos());

		if(!m->context_menu){
			init_context_menu();
		}

		m->context_menu->exec(pos);
	}

	if(event->button() & Qt::LeftButton){
		this->drag_pressed(event->pos());
	}
}

void FileListView::mouseMoveEvent(QMouseEvent* event)
{
	QDrag* drag = Dragable::drag_moving(event->pos());
	if(drag)
	{
		connect(drag, &QObject::destroyed, this, [=]()
{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}


void FileListView::init_context_menu()
{
	m->context_menu = new LibraryContextMenu(this);

	LibraryContexMenuEntries entries =
			(LibraryContextMenu::EntryDelete |
			LibraryContextMenu::EntryInfo |
			LibraryContextMenu::EntryAppend |
			LibraryContextMenu::EntryPlayNext);

	m->context_menu->show_actions(entries);

	connect(m->context_menu, &LibraryContextMenu::sig_info_clicked, this, &FileListView::sig_info_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_delete_clicked, this, &FileListView::sig_delete_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_play_next_clicked, this, &FileListView::sig_play_next_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_append_clicked, this, &FileListView::sig_append_clicked);
}


QModelIndexList FileListView::get_selected_rows() const
{
	QItemSelectionModel* selection_model;

	selection_model = this->selectionModel();

	if(selection_model){
		return selection_model->selectedIndexes();
	}

	return QModelIndexList();
}

MetaDataList FileListView::get_selected_metadata() const
{
	QStringList paths = get_selected_paths();
	DirectoryReader reader;

	return reader.get_md_from_filelist(paths);
}

QStringList FileListView::get_selected_paths() const
{
	QStringList paths = m->model->get_files();
	QStringList ret;
	QModelIndexList selections = this->get_selected_rows();

	for(const QModelIndex& idx : selections)
	{
		int row = idx.row();
		if(between(row, paths)){
			ret << paths[row];
		}
	}

	return ret;
}


void FileListView::set_parent_directory(const QString& dir)
{
	m->model->set_parent_directory(dir);
}

QMimeData*FileListView::get_mimedata() const
{
	QItemSelectionModel* sel_model = this->selectionModel();
	if(sel_model)
	{
		return m->model->mimeData(sel_model->selectedIndexes());
	}

	return nullptr;
}


int FileListView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex FileListView::get_model_index_by_index(int idx) const
{
	return m->model->index(idx);
}
