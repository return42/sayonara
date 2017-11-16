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
#include "DirectoryModel.h"

#include "Components/DirectoryReader/DirectoryReader.h"

#include "GUI/Utils/ContextMenu/LibraryContextMenu.h"
#include "GUI/Utils/CustomMimeData.h"
#include "GUI/Utils/MimeDataUtils.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Library/LibraryInfo.h"
#include "Utils/Utils.h"

#include <QDir>
#include <QMouseEvent>
#include <QDrag>

struct DirectoryTreeView::Private
{
	QString				last_search_term;

	LibraryContextMenu*	context_menu=nullptr;
	DirectoryModel*		model = nullptr;
	IconProvider*		icon_provider = nullptr;

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
	Gui::WidgetTemplate<SearchableTreeView>(parent),
	Dragable(this)
{
	m = Pimpl::make<Private>();

	QString root_path = Util::sayonara_path("Libraries");

	m->model = new DirectoryModel(this);
	m->model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	m->model->setIconProvider(m->icon_provider);
	m->model->setRootPath(root_path);

	this->setModel(m->model);
	this->setSearchModel(m->model);
	this->setItemDelegate(new DirectoryDelegate(this));


	for(int i=1; i<4; i++){
		this->hideColumn(i);
	}

	this->setRootIndex(m->model->index(root_path));
	connect(m->model, &DirectoryModel::directoryLoaded, this, &DirectoryTreeView::directory_loaded);
}

DirectoryTreeView::~DirectoryTreeView() {}

LibraryId DirectoryTreeView::library_id(const QModelIndex& index) const
{
	return m->model->library_id(index);
}


QMimeData* DirectoryTreeView::get_mimedata() const
{
	CustomMimeData* cmd	= new CustomMimeData(this);
	MetaDataList v_md = this->selected_metadata();

	cmd->set_metadata(v_md);

	return cmd;
}

void DirectoryTreeView::skin_changed()
{
	if(m && m->model){
		m->model->setIconProvider(m->icon_provider);
	}
}

void DirectoryTreeView::keyPressEvent(QKeyEvent* event)
{
	event->setAccepted(false);

	switch(event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			emit sig_enter_pressed();

			return;

		default: break;
	}

	m->model->search_only_dirs(true);

	SearchableTreeView::keyPressEvent(event);
}

void DirectoryTreeView::init_context_menu()
{
	m->context_menu = new LibraryContextMenu(this);

	LibraryContexMenuEntries entries =
			(LibraryContextMenu::EntryDelete |
			LibraryContextMenu::EntryInfo |
			LibraryContextMenu::EntryLyrics |
			LibraryContextMenu::EntryEdit |
			LibraryContextMenu::EntryAppend |
			LibraryContextMenu::EntryPlayNext);

	m->context_menu->show_actions(entries);

	connect(m->context_menu, &LibraryContextMenu::sig_info_clicked, this, &DirectoryTreeView::sig_info_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_lyrics_clicked, this, &DirectoryTreeView::sig_info_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_edit_clicked, this, &DirectoryTreeView::sig_info_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_delete_clicked, this, &DirectoryTreeView::sig_delete_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_play_next_clicked, this, &DirectoryTreeView::sig_play_next_clicked);
	connect(m->context_menu, &LibraryContextMenu::sig_append_clicked, this, &DirectoryTreeView::sig_append_clicked);
}

void DirectoryTreeView::directory_loaded(const QString& dir_name)
{
	QModelIndex index = m->model->index(dir_name);
	if(index.isValid())
	{
		scrollTo(index, QAbstractItemView::PositionAtCenter);
		selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
	}

	emit sig_directory_loaded(index);
}

QModelIndex DirectoryTreeView::search(const QString& search_term)
{
	QModelIndex found_idx;

	m->model->search_only_dirs(false);

	if(m->last_search_term == search_term) {
		found_idx = m->model->getNextRowIndexOf(m->last_search_term, 0, QModelIndex());
	}

	else {
		found_idx = m->model->getFirstRowIndexOf(search_term);
		m->last_search_term = search_term;
	}

	expand(found_idx);
	scrollTo(found_idx, QAbstractItemView::PositionAtCenter);
	selectionModel()->select(found_idx, QItemSelectionModel::ClearAndSelect);

	if(m->model->canFetchMore(found_idx)){
		m->model->fetchMore(found_idx);
	}

	return found_idx;
}

QString DirectoryTreeView::directory_name(const QModelIndex &index)
{
	QFileInfo info = m->model->fileInfo(index);
	return info.absoluteFilePath();
}

QModelIndexList DirectoryTreeView::selected_items() const
{
	QItemSelectionModel* selection_model = this->selectionModel();

	return selection_model->selectedRows();
}


MetaDataList DirectoryTreeView::selected_metadata() const
{
	DirectoryReader reader;
	QStringList paths = selected_paths();
	return reader.get_md_from_filelist(paths);
}


QStringList DirectoryTreeView::selected_paths() const
{
	QModelIndexList selections = this->selected_items();
	if(selections.isEmpty()){
		return QStringList();
	}

	QStringList paths;
	for(const QModelIndex& idx : selections)
	{
		paths << m->model->filepath_origin(idx);
	}

	return paths;
}



void DirectoryTreeView::select_match(const QString& str, SearchDirection direction)
{
	QModelIndex idx;

	if(direction == SearchDirection::First){
		idx = m->model->getFirstRowIndexOf(str);
	}

	else if(direction == SearchDirection::Next){
		idx = m->model->getNextRowIndexOf(str, 0);
	}

	else {
		idx = m->model->getPrevRowIndexOf(str, 0);
	}

	if(!idx.isValid()){
		sp_log(Log::Debug, this) << "Invalid index for " << str;
		return;
	}

	sp_log(Log::Debug, this) << "Selecting " << idx.data().toString();

	expand(idx);
	scrollTo(idx, QAbstractItemView::PositionAtCenter);
	this->clearSelection();
	selectionModel()->clearSelection();
	selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
	setCurrentIndex(idx);

	if(m->model->canFetchMore(idx)){
		m->model->fetchMore(idx);
	}
}

void DirectoryTreeView::mousePressEvent(QMouseEvent* event)
{
	QTreeView::mousePressEvent(event);

	if(event->buttons() & Qt::LeftButton)
	{
		Dragable::drag_pressed( event->pos() );
	}

	if(event->button() & Qt::RightButton){
		QPoint pos = QWidget::mapToGlobal( event->pos() );

		if(!m->context_menu){
			init_context_menu();
		}

		m->context_menu->exec(pos);
	}
}


void DirectoryTreeView::mouseMoveEvent(QMouseEvent* e)
{
	QDrag* drag = Dragable::drag_moving(e->pos());
	if(drag){
		connect(drag, &QDrag::destroyed, [=]()
{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}

void DirectoryTreeView::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept();
}

void DirectoryTreeView::dragMoveEvent(QDragMoveEvent* event)
{
	const QMimeData* mime_data = event->mimeData();
	const CustomMimeData* cmd = Gui::Util::MimeData::custom_mimedata(mime_data);
	if(cmd){
		event->setAccepted(false);
		return;
	}

	else{
		event->accept();
	}

	QModelIndex index = this->indexAt(event->pos());
	if(index.isValid()){
		this->expand(index);
		this->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
	}
}

void DirectoryTreeView::dropEvent(QDropEvent* event)
{
	event->accept();

	QModelIndex index = this->indexAt(event->pos());
	if(!index.isValid()){
		sp_log(Log::Debug, this) << "Drop: Invalid index";
		return;
	}

	const QMimeData* mime_data = event->mimeData();
	if(!mime_data){
		sp_log(Log::Debug, this) << "Drop: No Mimedata";
		return;
	}

	const CustomMimeData* cmd = Gui::Util::MimeData::custom_mimedata(mime_data);
	if(cmd)
	{
		if(cmd->has_source(this))
		{
			sp_log(Log::Debug, this) << "Drop: Internal item view drag";
		}
		else
		{
			sp_log(Log::Debug, this) << "Drop: Internal player drag";
		}

		return;
	}

	if(!mime_data->hasUrls())
	{
		sp_log(Log::Debug, this) << "Drop: No Urls";
		return;
	}

	LibraryId lib_id = m->model->library_id(index);
	QString target_dir = m->model->filepath_origin(index);

	QStringList files;

	for(const QUrl& url : mime_data->urls()){
		QString local_file = url.toLocalFile();
		if(!local_file.isEmpty()){
			files << local_file;
		}
	}

	sp_log(Log::Debug, this) << "Drop: " << files.size() << " files into library " << lib_id;

	if(lib_id < 0){
		return;
	}

	emit sig_import_requested(lib_id, files, target_dir);
}


int DirectoryTreeView::index_by_model_index(const QModelIndex& idx) const
{
	Q_UNUSED(idx)
	return -1;
}

QModelIndex DirectoryTreeView::model_index_by_index(int idx) const
{
	Q_UNUSED(idx)
	return QModelIndex();
}
