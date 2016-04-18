#include "FileListView.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "DirectoryIconProvider.h"
#include "DirectoryDelegate.h"
#include "Helper/Helper.h"
#include "Helper/DirectoryReader/DirectoryReader.h"


FileListView::FileListView(QWidget* parent) :
	SearchableListView(parent),
	SayonaraClass()
{
	QString lib_path = _settings->get(Set::Lib_Path);

	IconProvider* icon_provider = new IconProvider();

	_model = new QFileSystemModel(this);
	_model->setRootPath(lib_path);
	_model->setIconProvider(icon_provider);
	_model->setNameFilterDisables(false);
	_model->setNameFilters(Helper::get_soundfile_extensions() << Helper::get_playlistfile_extensions());
	_model->setFilter(QDir::Files);

	this->setModel(_model);
	this->setItemDelegate(new DirectoryDelegate(this));
	this->setRootIndex(_model->index(lib_path));
	this->setDragEnabled(true);
	this->setIconSize(QSize(16, 16));
}

void FileListView::mousePressEvent(QMouseEvent* event)
{

	SearchableListView::mousePressEvent(event);

	if(event->button() & Qt::RightButton){

		QPoint pos = QWidget::mapToGlobal(event->pos());

		if(!_context_menu){
			init_context_menu();
		}

		_context_menu->exec(pos);
	}
}

void FileListView::init_context_menu(){
	_context_menu = new LibraryContextMenu(this);

	LibraryContexMenuEntries entries =
			(LibraryContextMenu::EntryDelete |
			LibraryContextMenu::EntryInfo |
			LibraryContextMenu::EntryAppend |
			LibraryContextMenu::EntryPlayNext);

	_context_menu->show_actions(entries);

	connect(_context_menu, &LibraryContextMenu::sig_info_clicked, this, &FileListView::sig_info_clicked);
	connect(_context_menu, &LibraryContextMenu::sig_delete_clicked, this, &FileListView::sig_delete_clicked);
	connect(_context_menu, &LibraryContextMenu::sig_play_next_clicked, this, &FileListView::sig_play_next_clicked);
	connect(_context_menu, &LibraryContextMenu::sig_append_clicked, this, &FileListView::sig_append_clicked);
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


QFileSystemModel* FileListView::get_model() const
{
	return _model;
}


MetaDataList FileListView::read_metadata() const
{
	DirectoryReader reader;
	QModelIndexList idx_list = this->get_selected_rows();
	if(idx_list.isEmpty()){
		return MetaDataList();
	}

	QStringList paths;
	for(const QModelIndex& idx : idx_list){
		paths << _model->fileInfo(idx).absoluteFilePath();
	}

	return reader.get_md_from_filelist(paths);
}

