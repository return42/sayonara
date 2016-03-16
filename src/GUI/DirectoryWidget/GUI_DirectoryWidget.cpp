/* GUI_DirectoryWidget.cpp */

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



#include "GUI_DirectoryWidget.h"
#include "Components/Library/LocalLibrary.h"
#include "GUI/Helper/SearchableWidget/SearchableFileTreeView.h"

#include "Helper/DirectoryReader/DirectoryReader.h"
#include "DirectoryIconProvider.h"
#include "DirectoryDelegate.h"

#include <QItemSelectionModel>
#include <QApplication>

GUI_DirectoryWidget::GUI_DirectoryWidget(QWidget *parent) :
	SayonaraWidget(parent),
	Ui::GUI_DirectoryWidget()
{
	setupUi(this);

	_local_library = LocalLibrary::getInstance();

	connect(tv_dirs, &QTreeView::clicked, this, &GUI_DirectoryWidget::dir_clicked);
	connect(tv_dirs, &QTreeView::pressed, this, &GUI_DirectoryWidget::dir_pressed);
	connect(tv_dirs, &QTreeView::doubleClicked, this, &GUI_DirectoryWidget::dir_clicked);

	connect(lv_files, &QListView::doubleClicked, this, &GUI_DirectoryWidget::file_dbl_clicked);
	connect(lv_files, &QListView::pressed, this, &GUI_DirectoryWidget::file_pressed);

	connect(btn_search, &QPushButton::clicked, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(le_search, &QLineEdit::returnPressed, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(le_search, &QLineEdit::textChanged, this, &GUI_DirectoryWidget::search_term_changed);

	init_shortcuts();

	REGISTER_LISTENER(Set::Lib_Path, _sl_library_path_changed);

}

GUI_DirectoryWidget::~GUI_DirectoryWidget()
{
	if(_icon_provider){
		delete _icon_provider;
	}
}

QComboBox* GUI_DirectoryWidget::get_libchooser(){
	return combo_libchooser;
}


void GUI_DirectoryWidget::_sl_library_path_changed()
{
	if(_dir_model){
		_dir_model->setRootPath(_settings->get(Set::Lib_Path));
		_file_model->setRootPath(_settings->get(Set::Lib_Path));
	}
}



void GUI_DirectoryWidget::dir_clicked(QModelIndex idx){

	QString dir = _dir_model->fileInfo(idx).absoluteFilePath();
	QModelIndex tgt_idx = _file_model->setRootPath(dir);
	lv_files->setRootIndex(tgt_idx);
}

void GUI_DirectoryWidget::dir_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();
	if(buttons & Qt::MiddleButton){

		QStringList paths;
		QItemSelectionModel* selection_model;
		QModelIndexList selected_rows;

		selection_model = tv_dirs->selectionModel();
		selected_rows = selection_model->selectedRows();

		for(const QModelIndex& row_idx : selected_rows){

			QString path;
			QStringList tmp_paths;
			DirectoryReader reader;

			path = _dir_model->filePath(row_idx);
			reader.get_files_in_dir_rec(QDir(path), tmp_paths);
			paths << tmp_paths;
		}


		if(!paths.isEmpty()){
			_local_library->psl_prepare_tracks_for_playlist(paths, true);
		}
	}
}



void GUI_DirectoryWidget::file_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();
	if(buttons & Qt::MiddleButton){

		QStringList paths;
		QItemSelectionModel* selection_model;
		QModelIndexList selected_rows;

		selection_model = lv_files->selectionModel();

		selected_rows = selection_model->selectedIndexes();

		for(const QModelIndex& row_idx : selected_rows){
			paths << _file_model->filePath(row_idx);
		}

		if(!paths.isEmpty()){
			_local_library->psl_prepare_tracks_for_playlist(paths, true);
		}
	}
}



void GUI_DirectoryWidget::file_dbl_clicked(QModelIndex idx){
	QStringList paths;
	paths << _dir_model->filePath(idx);

	_local_library->psl_prepare_tracks_for_playlist(paths, false);
}


void GUI_DirectoryWidget::directory_loaded(const QString& path){

	Q_UNUSED(path)

	if(!_found_idx.isValid()){
		return;
	}

	tv_dirs->scrollTo(_found_idx, QAbstractItemView::PositionAtCenter);
	tv_dirs->selectionModel()->select(_found_idx, QItemSelectionModel::ClearAndSelect);
	dir_clicked(_found_idx);
}

void GUI_DirectoryWidget::files_loaded(const QString& path){

	QString searchstring = le_search->text();
	lv_files->clearSelection();

	if(searchstring.isEmpty()){
		return;
	}
	QModelIndex parent_idx = _file_model->index(path);
	int row_count = _file_model->rowCount(parent_idx);

	for(int row=0; row<row_count; row++){
		QModelIndex idx = _file_model->index(row, 0, parent_idx);
		QString text = idx.data().toString();
		if(text.contains(searchstring, Qt::CaseInsensitive)){
			lv_files->scrollTo(idx, QAbstractItemView::EnsureVisible);
			lv_files->selectionModel()->select(idx, QItemSelectionModel::Select);
		}
	}
}

void GUI_DirectoryWidget::search_button_clicked(){

	if(le_search->text().isEmpty()){
		return;
	}

	if(_search_term == le_search->text()){
		_found_idx = _dir_model->getNextRowIndexOf(_search_term, 0, QModelIndex());
	}
	else{
		_search_term = le_search->text();
		_found_idx = _dir_model->getFirstRowIndexOf(_search_term);
		btn_search->setText(tr("Search next"));
	}

	if(!_found_idx.isValid()){
		return;
	}

	if(_dir_model->canFetchMore(_found_idx)){
		_dir_model->fetchMore(_found_idx);
	}

	tv_dirs->scrollTo(_found_idx, QAbstractItemView::PositionAtCenter);
	tv_dirs->selectionModel()->select(_found_idx, QItemSelectionModel::ClearAndSelect);
	dir_clicked(_found_idx);
}

void GUI_DirectoryWidget::search_term_changed(const QString& term)
{
	if(term != _search_term && !term.isEmpty()){
		btn_search->setText(tr("Search"));
	}
}


void GUI_DirectoryWidget::init_dir_view(){

	QString lib_path = _settings->get(Set::Lib_Path);

	_icon_provider = new IconProvider();
	_dir_model = new AbstractSearchFileTreeModel(this);
	_file_model = new QFileSystemModel(this);

	_dir_model->setRootPath(lib_path);
	_dir_model->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
	_dir_model->setIconProvider(_icon_provider);

	_file_model->setRootPath(lib_path);
	_file_model->setIconProvider(_icon_provider);
	_file_model->setNameFilterDisables(false);
	_file_model->setNameFilters(Helper::get_soundfile_extensions() << Helper::get_playlistfile_extensions());
	_file_model->setFilter(QDir::Files);

	tv_dirs->setModel(_dir_model);
	tv_dirs->setItemDelegate(new DirectoryDelegate(this));
	tv_dirs->setRootIndex(_dir_model->index(lib_path));
	tv_dirs->setDragEnabled(true);

	for(int i=1; i<4; i++){
		tv_dirs->hideColumn(i);
	}

	lv_files->setModel(_file_model);
	lv_files->setItemDelegate(new DirectoryDelegate(this));
	lv_files->setRootIndex(_file_model->index(lib_path));
	lv_files->setDragEnabled(true);
	lv_files->setIconSize(QSize(16, 16));

	connect(_dir_model, &AbstractSearchFileTreeModel::directoryLoaded,
			this, &GUI_DirectoryWidget::directory_loaded);

	connect(_file_model, &QFileSystemModel::directoryLoaded,
			this, &GUI_DirectoryWidget::files_loaded);

}


void GUI_DirectoryWidget::showEvent(QShowEvent* e){

	if(!_dir_model){
		init_dir_view();
	}

	e->accept();
}

void GUI_DirectoryWidget::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), le_search, SLOT(setFocus()), nullptr, Qt::WindowShortcut);
	new QShortcut(QKeySequence("Esc"), le_search, SLOT(clear()), nullptr, Qt::WidgetShortcut);
}


DirectoryLibraryContainer::DirectoryLibraryContainer(QObject* parent) :
	LibraryContainerInterface(parent)
{

}

QString DirectoryLibraryContainer::get_name() const
{
	return "directories";
}

QString DirectoryLibraryContainer::get_display_name() const
{
	return tr("Directories");
}

QIcon DirectoryLibraryContainer::get_icon() const
{
	return Helper::get_icon("dir_view");
}

QWidget* DirectoryLibraryContainer::get_ui() const
{
	return static_cast<QWidget*>(ui);
}

QComboBox*DirectoryLibraryContainer::get_libchooser()
{
	return ui->get_libchooser();
}

void DirectoryLibraryContainer::init_ui()
{
	ui = new GUI_DirectoryWidget(nullptr);
}
