/* DirectoryTreeView.cpp */

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

#include "DirectoryTreeView.h"
#include "DirectoryDelegate.h"
#include "DirectoryIconProvider.h"

#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/SearchableWidget/SearchableFileTreeModel.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Library/LibraryInfo.h"

#include <QDir>
#include <QMouseEvent>
#include <QDrag>

struct DirectoryTreeView::Private
{
	LibraryContextMenu*			context_menu=nullptr;
	SearchableFileTreeModel*	model = nullptr;
	IconProvider*				icon_provider = nullptr;

	Private()
	{
		icon_provider = new IconProvider();
	}

	~Private()
	{
		delete icon_provider; icon_provider = nullptr;
	}
};


DirectoryTreeView::DirectoryTreeView(QWidget *parent) :
	SearchableTreeView(parent),
	Dragable(this)
{
	_m = Pimpl::make<Private>();

	_m->model = new SearchableFileTreeModel(this);
	_m->model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	_m->model->setIconProvider(_m->icon_provider);

	this->setModel(_m->model);
	this->setItemDelegate(new DirectoryDelegate(this));
	this->setDragEnabled(true);

	for(int i=1; i<4; i++){
		this->hideColumn(i);
	}
}

DirectoryTreeView::~DirectoryTreeView() {}


void DirectoryTreeView::mousePressEvent(QMouseEvent* event)
{
	QTreeView::mousePressEvent(event);

	if(event->buttons() & Qt::LeftButton)
	{
		Dragable::drag_pressed( event->pos() );
	}

	if(event->button() & Qt::RightButton){
		QPoint pos = QWidget::mapToGlobal( event->pos() );

		if(!_m->context_menu){
			init_context_menu();
		}

		_m->context_menu->exec(pos);
	}
}


void DirectoryTreeView::mouseMoveEvent(QMouseEvent* e)
{
	QDrag* drag = Dragable::drag_moving(e->pos());
	if(drag){
		connect(drag, &QDrag::destroyed, this, [=]()
{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}

QMimeData* DirectoryTreeView::get_mimedata() const
{
	QItemSelectionModel* sel_model = this->selectionModel();
	if(sel_model)
	{
		return _m->model->mimeData(sel_model->selectedIndexes());
	}

	return nullptr;
}
void DirectoryTreeView::init_context_menu()
{
	_m->context_menu = new LibraryContextMenu(this);

	LibraryContexMenuEntries entries =
			(LibraryContextMenu::EntryDelete |
			LibraryContextMenu::EntryInfo |
			LibraryContextMenu::EntryAppend |
			LibraryContextMenu::EntryPlayNext);

	_m->context_menu->show_actions(entries);

	connect(_m->context_menu, &LibraryContextMenu::sig_info_clicked, this, &DirectoryTreeView::sig_info_clicked);
	connect(_m->context_menu, &LibraryContextMenu::sig_delete_clicked, this, &DirectoryTreeView::sig_delete_clicked);
	connect(_m->context_menu, &LibraryContextMenu::sig_play_next_clicked, this, &DirectoryTreeView::sig_play_next_clicked);
	connect(_m->context_menu, &LibraryContextMenu::sig_append_clicked, this, &DirectoryTreeView::sig_append_clicked);
}

SearchableFileTreeModel* DirectoryTreeView::get_model() const
{
	return _m->model;
}


QModelIndexList DirectoryTreeView::get_selected_rows() const
{
	QItemSelectionModel* selection_model = this->selectionModel();

	return selection_model->selectedRows();
}


MetaDataList DirectoryTreeView::get_selected_metadata() const
{
	DirectoryReader reader;
	QStringList paths = get_selected_paths();
	return reader.get_md_from_filelist(paths);
}


QStringList DirectoryTreeView::get_selected_paths() const
{
	QModelIndexList idx_list = this->get_selected_rows();
	if(idx_list.isEmpty()){
		return QStringList();
	}

	QStringList paths;
	for(const QModelIndex& idx : idx_list){
		paths << _m->model->fileInfo(idx).absoluteFilePath();
	}

	return paths;
}

int DirectoryTreeView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex DirectoryTreeView::get_model_index_by_index(int idx) const
{
	return _m->model->index(idx, 0);
}

void DirectoryTreeView::set_cur_library(const QString& library_path)
{
	_m->model->setRootPath(library_path);
	this->setRootIndex(_m->model->index(library_path));
}
