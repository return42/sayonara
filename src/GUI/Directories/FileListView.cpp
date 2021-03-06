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
#include "DirectoryIconProvider.h"
#include "DirectoryDelegate.h"
#include "DirectoryContextMenu.h"

#include "Components/Directories/DirectoryReader.h"
#include "Components/Directories/FileOperations.h"

#include "Utils/globals.h"
#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/FileUtils.h"
#include "Utils/Language.h"

#include "GUI/Utils/InputDialog/LineInputDialog.h"
#include "GUI/Utils/PreferenceAction.h"
#include "GUI/Utils/CustomMimeData.h"
#include "GUI/Utils/MimeDataUtils.h"
#include "GUI/Utils/Icons.h"

#include <QDir>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QPainter>
#include <QMimeData>
#include <QApplication>


struct FileListView::Private
{
	DirectoryContextMenu*	context_menu=nullptr;
	FileListModel*			model=nullptr;
	FileOperations*			file_operations=nullptr;

	Private(FileListView* parent)
	{
		model = new FileListModel(parent);
		file_operations = new FileOperations(parent);
	}
};

FileListView::FileListView(QWidget* parent) :
	Gui::WidgetTemplate<SearchableListView>(parent),
	Dragable(this)
{
	m = Pimpl::make<Private>(this);

	this->setModel(m->model);
	this->set_search_model(m->model);
	this->setItemDelegate(new DirectoryDelegate(this));
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setDragEnabled(true);
	this->setIconSize(QSize(16, 16));

	QAction* action = new QAction(this);
	connect(action, &QAction::triggered, this, &FileListView::rename_file_clicked);
	action->setShortcut(QKeySequence("F2"));
	action->setShortcutContext(Qt::WidgetShortcut);
	this->addAction(action);
}

FileListView::~FileListView() {}

void FileListView::mousePressEvent(QMouseEvent* event)
{
	SearchableListView::mousePressEvent(event);

	if(event->button() & Qt::RightButton)
	{
		QPoint pos = QWidget::mapToGlobal(event->pos());

		if(!m->context_menu){
			init_context_menu();
		}

		m->context_menu->show_action(
			LibraryContextMenu::EntryLyrics,
			(selected_rows().size()==1)
		);

		m->context_menu->set_rename_visible(
			(selected_rows().size()==1)
		);

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
		connect(drag, &QObject::destroyed, [=]()
{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}

void FileListView::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void FileListView::dragMoveEvent(QDragMoveEvent *event)
{
	const QMimeData* mime_data = event->mimeData();
	const CustomMimeData* cmd = Gui::MimeData::custom_mimedata(mime_data);
	if(cmd){
		event->setAccepted(false);
	}

	else{
		event->setAccepted(true);
	}
}

void FileListView::dropEvent(QDropEvent *event)
{
	event->accept();

	if(m->model->library_id() < 0){
		return;
	}

	const QMimeData* mime_data = event->mimeData();
	if(!mime_data){
		sp_log(Log::Debug, this) << "Drop: No Mimedata";
		return;
	}

	if(Gui::MimeData::is_player_drag(mime_data)){
		sp_log(Log::Debug, this) << "Drop: Internal player drag";
		return;
	}

	if(!mime_data->hasUrls())
	{
		sp_log(Log::Debug, this) << "Drop: No Urls";
		return;
	}

	QStringList files;
	for(const QUrl& url : mime_data->urls())
	{
		QString local_file = url.toLocalFile();
		if(!local_file.isEmpty()){
			files << local_file;
		}
	}

	sp_log(Log::Debug, this) << "Drop: " << files.size() << " files into library " << m->model->library_id();

	emit sig_import_requested(m->model->library_id(), files, m->model->parent_directory());
}

void FileListView::language_changed() {}
void FileListView::skin_changed() {}

void FileListView::init_context_menu()
{
	if(m->context_menu){
		return;
	}

	m->context_menu = new DirectoryContextMenu(DirectoryContextMenu::Mode::File, this);

	connect(m->context_menu, &DirectoryContextMenu::sig_info_clicked, this, &FileListView::sig_info_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_lyrics_clicked, this, &FileListView::sig_lyrics_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_edit_clicked, this, &FileListView::sig_edit_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_delete_clicked, this, &FileListView::sig_delete_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_play_clicked, this, &FileListView::sig_play_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_play_new_tab_clicked, this, &FileListView::sig_play_new_tab_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_play_next_clicked, this, &FileListView::sig_play_next_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_append_clicked, this, &FileListView::sig_append_clicked);
	connect(m->context_menu, &DirectoryContextMenu::sig_rename_clicked, this, &FileListView::rename_file_clicked);
}



QModelIndexList FileListView::selected_rows() const
{
	QItemSelectionModel* selection_model;

	selection_model = this->selectionModel();

	if(selection_model){
		return selection_model->selectedIndexes();
	}

	return QModelIndexList();
}

MetaDataList FileListView::selected_metadata() const
{
	QStringList paths = selected_paths();
	DirectoryReader reader;

	return reader.metadata_from_filelist(paths);
}

QStringList FileListView::selected_paths() const
{
	QStringList paths = m->model->files();
	QStringList ret;
	QModelIndexList selections = this->selected_rows();

	for(const QModelIndex& idx : selections)
	{
		int row = idx.row();
		if(between(row, paths)){
			ret << paths[row];
		}
	}

	return ret;
}


void FileListView::set_parent_directory(LibraryId library_id, const QString& dir)
{
	this->selectionModel()->clear();

	m->model->set_parent_directory(library_id, dir);
}

QString FileListView::parent_directory() const
{
	return m->model->parent_directory();
}

void FileListView::set_search_filter(const QString& search_string)
{
	if(search_string.isEmpty()){
		return;
	}

	Library::SearchModeMask smm = _settings->get(Set::Lib_SearchMode);
	QString search_text = Library::Util::convert_search_string(search_string, smm);

	int n_rows = m->model->rowCount();
	for(int i=0; i<n_rows; i++)
	{
		QModelIndex idx = m->model->index(i, 0);
		QString data = m->model->data(idx).toString();
		if(data.isEmpty()){
			continue;
		}

		if(!idx.isValid()){
			continue;
		}

		data = Library::Util::convert_search_string(data, smm);
		if(data.contains(search_text, Qt::CaseInsensitive)){
			this->selectionModel()->select(idx, (QItemSelectionModel::Select | QItemSelectionModel::Rows));
		}
	}
}

QMimeData* FileListView::dragable_mimedata() const
{
	CustomMimeData* mimedata = new CustomMimeData(this);
	mimedata->set_metadata(selected_metadata());
	return mimedata;
}


int FileListView::index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex FileListView::model_index_by_index(int idx) const
{
	return m->model->index(idx);
}

void FileListView::keyPressEvent(QKeyEvent *event)
{
	event->setAccepted(false);

	if( event->key() == Qt::Key_Enter ||
		event->key() == Qt::Key_Return)
	{
		event->accept();
		emit sig_enter_pressed();
	}

	SearchableListView::keyPressEvent(event);
}


void FileListView::rename_file_clicked()
{
	QModelIndexList indexes = this->selected_rows();
	if(indexes.size() != 1){
		return;
	}

	QModelIndex index = indexes.first();
	int row = index.row();

	QStringList files = m->model->files();
	if(!between(row, files)){
		return;
	}

	QString file = Util::File::get_filename_of_path(files[row]);
	QString ext = Util::File::calc_file_extension(file);

	int last_dot = file.lastIndexOf(".");
	file = file.left(last_dot);

	LineInputDialog dialog(Lang::get(Lang::Rename), tr("Enter new name"), file, this);
	dialog.exec();

	QString file_renamed = dialog.textValue();

	if(!file_renamed.isEmpty())
	{

		QDir d(files[row]);
		d.cdUp();
		QString new_filename = d.filePath(file_renamed) + "." + ext;

		sp_log(Log::Debug, this) << "Will rename " << files[row] << " to " << new_filename;

		m->file_operations->rename_file(files[row], new_filename);
		m->model->set_parent_directory(m->model->library_id(), m->model->parent_directory());

		files = m->model->files();
		int new_file_index = files.indexOf(new_filename);
		if(between(new_file_index, files)){
			this->select_row(new_file_index);
		}
	}
}
