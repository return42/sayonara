/* GUI_DirectoryWidget.cpp */

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

#include "GUI_DirectoryWidget.h"
#include "FileListModel.h"
#include "DirectoryDelegate.h"
#include "GUI/DirectoryWidget/ui_GUI_DirectoryWidget.h"

#include "GUI/Helper/SearchableWidget/SearchableFileTreeModel.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"

#include "Components/Library/LibraryManager.h"
#include "Components/Library/LocalLibrary.h"
#include "Components/Playlist/PlaylistHandler.h"

#include "Helper/Message/GlobalMessage.h"
#include "Helper/Library/LibraryNamespaces.h"
#include "Helper/DirectoryReader/DirectoryReader.h"
#include "Helper/FileHelper.h"
#include "Helper/globals.h"
#include "Helper/Language.h"


#include <QItemSelectionModel>
#include <QApplication>
#include <QMouseEvent>
#include <QShortcut>

struct GUI_DirectoryWidget::Private
{
	enum SelectedWidget
	{
		None=0,
		Dirs,
		Files
	} selected_widget;

	IconProvider*						icon_provider=nullptr;
	LocalLibrary*						local_library=nullptr;
	SearchableFileTreeModel*			dir_model=nullptr;
	QModelIndex							found_idx;
	QString								search_term;
	QStringList							found_strings;
};

GUI_DirectoryWidget::GUI_DirectoryWidget(QWidget *parent) :
	SayonaraWidget(parent),
	InfoDialogContainer()
{
	ui = new Ui::GUI_DirectoryWidget();
	ui->setupUi(this);

	m = Pimpl::make<GUI_DirectoryWidget::Private>();

	m->selected_widget = Private::SelectedWidget::None;
	m->local_library = LibraryManager::getInstance()->library_instance(0);
	m->dir_model = ui->tv_dirs->get_model();

	connect(ui->tv_dirs, &QTreeView::clicked, this, &GUI_DirectoryWidget::dir_clicked);
	connect(ui->tv_dirs, &QTreeView::pressed, this, &GUI_DirectoryWidget::dir_pressed);
	connect(ui->tv_dirs, &QTreeView::doubleClicked, this, &GUI_DirectoryWidget::dir_clicked);

	connect(ui->lv_files, &QListView::doubleClicked, this, &GUI_DirectoryWidget::file_dbl_clicked);
	connect(ui->lv_files, &QListView::pressed, this, &GUI_DirectoryWidget::file_pressed);

	connect(ui->btn_search, &QPushButton::clicked, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(ui->le_search, &QLineEdit::returnPressed, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(ui->le_search, &QLineEdit::textChanged, this, &GUI_DirectoryWidget::search_term_changed);

	connect(ui->tv_dirs, &DirectoryTreeView::sig_info_clicked, this, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Dirs;
		show_info();
	});

	connect(ui->tv_dirs, &DirectoryTreeView::sig_append_clicked, this, &GUI_DirectoryWidget::dir_append_clicked);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_play_next_clicked, this, &GUI_DirectoryWidget::dir_play_next_clicked);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_delete_clicked, this, &GUI_DirectoryWidget::dir_delete_clicked);

	connect(ui->lv_files, &FileListView::sig_info_clicked, this, [=]()
{
		m->selected_widget = Private::SelectedWidget::Files;
		show_info();
	});

	connect(ui->lv_files, &FileListView::sig_append_clicked, this, &GUI_DirectoryWidget::file_append_clicked);
	connect(ui->lv_files, &FileListView::sig_play_next_clicked, this, &GUI_DirectoryWidget::file_play_next_clicked);
	connect(ui->lv_files, &FileListView::sig_delete_clicked, this, &GUI_DirectoryWidget::file_delete_clicked);

	init_shortcuts();
}

GUI_DirectoryWidget::~GUI_DirectoryWidget()
{
	if(ui){
		delete ui; ui = nullptr;
	}
}

QFrame* GUI_DirectoryWidget::header_frame() const
{
	return ui->header_frame;
}


MD::Interpretation GUI_DirectoryWidget::metadata_interpretation() const
{
	return MD::Interpretation::Tracks;
}


MetaDataList GUI_DirectoryWidget::info_dialog_data() const
{
	MetaDataList v_md;

	switch(m->selected_widget)
	{
		case Private::SelectedWidget::Dirs:
			return ui->tv_dirs->get_selected_metadata();
		case Private::SelectedWidget::Files:
			return ui->lv_files->get_selected_metadata();
		default:
			return v_md;
	}
}


void GUI_DirectoryWidget::dir_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();

	if(buttons & Qt::MiddleButton)
	{
		QStringList paths = ui->tv_dirs->get_selected_paths();

		if(!paths.isEmpty()){
			m->local_library->prepare_tracks_for_playlist(paths, true);
		}
	}
}


void GUI_DirectoryWidget::dir_clicked(QModelIndex idx)
{
	QString dir = m->dir_model->fileInfo(idx).absoluteFilePath();
	ui->lv_files->set_parent_directory(dir);
}


void GUI_DirectoryWidget::dir_append_clicked()
{
	MetaDataList v_md = ui->tv_dirs->get_selected_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->append_tracks(v_md, plh->get_current_idx());
}


void GUI_DirectoryWidget::dir_play_next_clicked()
{
	MetaDataList v_md = ui->tv_dirs->get_selected_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->play_next(v_md);
}


void GUI_DirectoryWidget::dir_delete_clicked()
{
	GlobalMessage* gm = GlobalMessage::getInstance();
	GlobalMessage::Answer answer = gm->question(Lang::get(Lang::Really) + "?");

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	QStringList files = ui->tv_dirs->get_selected_paths();
	MetaDataList v_md = ui->tv_dirs->get_selected_metadata();

	m->local_library->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);

	Helper::File::delete_files(files);
}


void GUI_DirectoryWidget::file_append_clicked()
{
	MetaDataList v_md = ui->lv_files->get_selected_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->append_tracks(v_md, plh->get_current_idx());
}


void GUI_DirectoryWidget::file_play_next_clicked()
{
	MetaDataList v_md = ui->lv_files->get_selected_metadata();
	PlaylistHandler* plh = PlaylistHandler::getInstance();
	plh->play_next(v_md);
}


void GUI_DirectoryWidget::file_delete_clicked()
{
	GlobalMessage* gm = GlobalMessage::getInstance();
	GlobalMessage::Answer answer = gm->question(Lang::get(Lang::Really) + "?");

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	MetaDataList v_md = ui->lv_files->get_selected_metadata();

	m->local_library->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);

	QStringList files = ui->lv_files->get_selected_paths();
	Helper::File::delete_files(files);
}

void GUI_DirectoryWidget::language_changed()
{
	ui->btn_search->setText(Lang::get(Lang::Search));
}


void GUI_DirectoryWidget::file_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();

	if(buttons & Qt::MiddleButton)
	{
		QStringList paths = ui->lv_files->get_selected_paths();
		m->local_library->prepare_tracks_for_playlist(paths, true);
	}
}


void GUI_DirectoryWidget::file_dbl_clicked(QModelIndex idx)
{
	Q_UNUSED(idx)

	QStringList paths = ui->lv_files->get_selected_paths();
	m->local_library->prepare_tracks_for_playlist(paths, false);
}

void GUI_DirectoryWidget::directory_loaded(const QString& path){
	Q_UNUSED(path)

	if(!m->found_idx.isValid()){
		return;
	}

	ui->tv_dirs->scrollTo(m->found_idx, QAbstractItemView::PositionAtCenter);
	ui->tv_dirs->selectionModel()->select(m->found_idx, QItemSelectionModel::ClearAndSelect);

	dir_clicked(m->found_idx);
}


void GUI_DirectoryWidget::search_button_clicked()
{
	if(ui->le_search->text().isEmpty()){
		return;
	}

	if(m->search_term == ui->le_search->text()){
		m->found_idx = m->dir_model->getNextRowIndexOf(m->search_term, 0, QModelIndex());
	}
	else{
		m->search_term = ui->le_search->text();
		m->found_idx = m->dir_model->getFirstRowIndexOf(m->search_term);
		ui->btn_search->setText(Lang::get(Lang::SearchNext));
	}

	if(!m->found_idx.isValid()){
		return;
	}

	if(m->dir_model->canFetchMore(m->found_idx)){
		m->dir_model->fetchMore(m->found_idx);
	}

	ui->tv_dirs->scrollTo(m->found_idx, QAbstractItemView::PositionAtCenter);
	ui->tv_dirs->selectionModel()->select(m->found_idx, QItemSelectionModel::ClearAndSelect);
	dir_clicked(m->found_idx);
}

void GUI_DirectoryWidget::search_term_changed(const QString& term)
{
	if(term != m->search_term && !term.isEmpty()){
		ui->btn_search->setText(Lang::get(Lang::Search));
	}
}


void GUI_DirectoryWidget::init_dir_view()
{
	connect(m->dir_model, &SearchableFileTreeModel::directoryLoaded,
			this, &GUI_DirectoryWidget::directory_loaded);
}


void GUI_DirectoryWidget::showEvent(QShowEvent* e)
{
	if(!m->dir_model){
		init_dir_view();
	}

	SayonaraWidget::showEvent(e);
}


void GUI_DirectoryWidget::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), ui->le_search, SLOT(setFocus()), nullptr, Qt::WindowShortcut);
	new QShortcut(QKeySequence("Esc"), ui->le_search, SLOT(clear()), nullptr, Qt::WidgetShortcut);
}
