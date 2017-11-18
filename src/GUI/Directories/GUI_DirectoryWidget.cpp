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
#include "DirectoryModel.h"

#include "GUI/Directories/ui_GUI_DirectoryWidget.h"

#include "GUI/ImportDialog/GUI_ImportDialog.h"
#include "GUI/Utils/ContextMenu/LibraryContextMenu.h"

#include "Components/Library/LibraryManager.h"
#include "Components/Library/LocalLibrary.h"
#include "Components/Playlist/PlaylistHandler.h"
#include "Components/DirectoryReader/DirectoryReader.h"

#include "Utils/Message/GlobalMessage.h"
#include "Utils/Library/LibraryNamespaces.h"
#include "Utils/Library/LibraryInfo.h"
#include "Utils/FileUtils.h"
#include "Utils/globals.h"
#include "Utils/Language.h"

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

	QList<LocalLibrary*> local_libraries;
	bool	is_search_active;

	Private() :
		selected_widget(None),
		is_search_active(false)
	{}
};

GUI_DirectoryWidget::GUI_DirectoryWidget(QWidget *parent) :
	Widget(parent),
	InfoDialogContainer()
{
	ui = new Ui::GUI_DirectoryWidget();
	ui->setupUi(this);

	m = Pimpl::make<GUI_DirectoryWidget::Private>();

	m->selected_widget = Private::SelectedWidget::None;
	Library::Manager* library_manager = Library::Manager::instance();
	QList<Library::Info> all_libraries = library_manager->all_libraries();

	for(const Library::Info& info : all_libraries)
	{
		LocalLibrary* l = library_manager->library_instance(info.id());
		if(l){
			m->local_libraries << l;
		}
	}

	connect(ui->btn_search, &QPushButton::clicked, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(ui->le_search, &QLineEdit::returnPressed, this, &GUI_DirectoryWidget::search_button_clicked);
	connect(ui->le_search, &QLineEdit::textChanged, this, &GUI_DirectoryWidget::search_text_edited);

	ui->tv_dirs->setExpandsOnDoubleClick(true);
	ui->tv_dirs->setDragEnabled(true);
	ui->tv_dirs->setAcceptDrops(true);
	ui->tv_dirs->setDragDropMode(QAbstractItemView::DragDrop);
	ui->tv_dirs->setDropIndicatorShown(true);

	ui->lv_files->setDragEnabled(true);
	ui->lv_files->setAcceptDrops(true);
	ui->lv_files->setDragDropMode(QAbstractItemView::DragDrop);
	ui->lv_files->setDropIndicatorShown(true);

	connect(ui->tv_dirs, &QTreeView::clicked, this, &GUI_DirectoryWidget::dir_opened);
	connect(ui->tv_dirs, &QTreeView::pressed, this, &GUI_DirectoryWidget::dir_pressed);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_import_requested, this, &GUI_DirectoryWidget::import_requested);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_enter_pressed, this, &GUI_DirectoryWidget::dir_enter_pressed);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_append_clicked, this, &GUI_DirectoryWidget::dir_append_clicked);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_play_next_clicked, this, &GUI_DirectoryWidget::dir_play_next_clicked);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_delete_clicked, this, &GUI_DirectoryWidget::dir_delete_clicked);
	connect(ui->tv_dirs, &DirectoryTreeView::sig_directory_loaded, this, &GUI_DirectoryWidget::dir_opened);

	connect(ui->tv_dirs, &DirectoryTreeView::sig_info_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Dirs;
		show_info();
	});

	connect(ui->tv_dirs, &DirectoryTreeView::sig_edit_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Dirs;
		show_edit();
	});

	connect(ui->tv_dirs, &DirectoryTreeView::sig_lyrics_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Dirs;
		show_lyrics();
	});

	connect(ui->lv_files, &QListView::doubleClicked, this, &GUI_DirectoryWidget::file_dbl_clicked);
	connect(ui->lv_files, &QListView::pressed, this, &GUI_DirectoryWidget::file_pressed);
	connect(ui->lv_files, &FileListView::sig_import_requested, this, &GUI_DirectoryWidget::import_requested);
	connect(ui->lv_files, &FileListView::sig_enter_pressed, this, &GUI_DirectoryWidget::file_enter_pressed);
	connect(ui->lv_files, &FileListView::sig_append_clicked, this, &GUI_DirectoryWidget::file_append_clicked);
	connect(ui->lv_files, &FileListView::sig_play_next_clicked, this, &GUI_DirectoryWidget::file_play_next_clicked);
	connect(ui->lv_files, &FileListView::sig_delete_clicked, this, &GUI_DirectoryWidget::file_delete_clicked);
	connect(ui->lv_files, &FileListView::sig_info_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Files;
		show_info();
	});

	connect(ui->lv_files, &FileListView::sig_edit_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Files;
		show_edit();
	});

	connect(ui->lv_files, &FileListView::sig_lyrics_clicked, [=]()
	{
		m->selected_widget = Private::SelectedWidget::Files;
		show_lyrics();
	});

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
			return ui->tv_dirs->selected_metadata();
		case Private::SelectedWidget::Files:
			return ui->lv_files->selected_metadata();
		default:
			return v_md;
	}
}


void GUI_DirectoryWidget::dir_enter_pressed()
{
	QModelIndexList indexes = ui->tv_dirs->selected_items();
	if(!indexes.isEmpty()){
		ui->tv_dirs->expand(indexes.first());
	}
}


void GUI_DirectoryWidget::dir_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();

	if(buttons & Qt::MiddleButton)
	{
		QStringList paths = ui->tv_dirs->selected_paths();

		if(!paths.isEmpty())
		{
			LocalLibrary* l = m->local_libraries.first();
			l->prepare_tracks_for_playlist(paths, true);
		}
	}
}

void GUI_DirectoryWidget::dir_opened(QModelIndex idx)
{
	QString dir = ui->tv_dirs->directory_name_origin(idx);

	ui->lv_files->set_parent_directory(ui->tv_dirs->library_id(idx), dir);
	ui->lv_files->set_search_filter(ui->le_search->text());
}


void GUI_DirectoryWidget::dir_append_clicked()
{
	MetaDataList v_md = ui->tv_dirs->selected_metadata();
	Playlist::Handler* plh = Playlist::Handler::instance();
	plh->append_tracks(v_md, plh->current_index());
}


void GUI_DirectoryWidget::dir_play_next_clicked()
{
	MetaDataList v_md = ui->tv_dirs->selected_metadata();
	Playlist::Handler* plh = Playlist::Handler::instance();
	plh->play_next(v_md);
}


void GUI_DirectoryWidget::dir_delete_clicked()
{
	GlobalMessage::Answer answer = GlobalMessage::question(Lang::get(Lang::Really) + "?");

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	QStringList files = ui->tv_dirs->selected_paths();
	MetaDataList v_md = ui->tv_dirs->selected_metadata();

	LocalLibrary* l = m->local_libraries.first();
	l->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);

	Util::File::delete_files(files);
}


void GUI_DirectoryWidget::file_append_clicked()
{
	MetaDataList v_md = ui->lv_files->selected_metadata();
	Playlist::Handler* plh = Playlist::Handler::instance();
	plh->append_tracks(v_md, plh->current_index());
}


void GUI_DirectoryWidget::file_play_next_clicked()
{
	MetaDataList v_md = ui->lv_files->selected_metadata();
	Playlist::Handler* plh = Playlist::Handler::instance();
	plh->play_next(v_md);
}


void GUI_DirectoryWidget::file_delete_clicked()
{
	GlobalMessage::Answer answer = GlobalMessage::question(Lang::get(Lang::Really) + "?");

	if(answer != GlobalMessage::Answer::Yes){
		return;
	}

	MetaDataList v_md = ui->lv_files->selected_metadata();

	LocalLibrary* l = m->local_libraries.first();
	l->delete_tracks(v_md, Library::TrackDeletionMode::OnlyLibrary);

	QStringList files = ui->lv_files->selected_paths();
	Util::File::delete_files(files);
}

void GUI_DirectoryWidget::import_requested(LibraryId id, const QStringList& paths, const QString& target_dir)
{
	Library::Manager* library_manager = Library::Manager::instance();
	LocalLibrary* library = library_manager->library_instance(id);
	if(!library){
		return;
	}

	connect(library, &LocalLibrary::sig_import_dialog_requested,
			this, &GUI_DirectoryWidget::import_dialog_requested);

	library->import_files(paths, target_dir);
}

void GUI_DirectoryWidget::import_dialog_requested(const QString& target_dir)
{
	if(!this->isVisible()){
		return;
	}

	LocalLibrary* library = dynamic_cast<LocalLibrary*>(sender());
	if(!library){
		return;
	}

	GUI_ImportDialog* importer = new GUI_ImportDialog(library, true, this);

	connect(importer, &GUI_ImportDialog::sig_closed, importer, &GUI_ImportDialog::deleteLater);

	importer->set_target_dir(target_dir);
	importer->show();
}

void GUI_DirectoryWidget::file_pressed(QModelIndex idx)
{
	Q_UNUSED(idx)

	Qt::MouseButtons buttons = QApplication::mouseButtons();

	if(buttons & Qt::MiddleButton)
	{
		QStringList paths = ui->lv_files->selected_paths();

		LocalLibrary* l = m->local_libraries.first();
		l->prepare_tracks_for_playlist(paths, true);
	}
}


void GUI_DirectoryWidget::file_dbl_clicked(QModelIndex idx)
{
	Q_UNUSED(idx)

	QStringList paths = ui->lv_files->selected_paths();

	LocalLibrary* l = m->local_libraries.first();
	l->prepare_tracks_for_playlist(paths, false);
}

void GUI_DirectoryWidget::file_enter_pressed()
{
	QStringList paths = ui->lv_files->selected_paths();

	LocalLibrary* l = m->local_libraries.first();
	l->prepare_tracks_for_playlist(paths, false);
}

void GUI_DirectoryWidget::search_button_clicked()
{
	if(ui->le_search->text().isEmpty()){
		m->is_search_active	= true;
		return;
	}

	QModelIndex found_idx = ui->tv_dirs->search(ui->le_search->text());
	if(found_idx.isValid()){
		dir_opened(found_idx);
		ui->btn_search->setText(Lang::get(Lang::SearchNext));
		m->is_search_active	= true;
	}
}

void GUI_DirectoryWidget::search_text_edited(const QString& text)
{
	Q_UNUSED(text)
	m->is_search_active = false;
	ui->btn_search->setText(Lang::get(Lang::Search));
}


void GUI_DirectoryWidget::init_shortcuts()
{
	new QShortcut(QKeySequence("Ctrl+f"), ui->le_search, SLOT(setFocus()), nullptr, Qt::WindowShortcut);
	new QShortcut(QKeySequence("Esc"), ui->le_search, SLOT(clear()), nullptr, Qt::WidgetShortcut);
}

void GUI_DirectoryWidget::language_changed()
{
	if(m->is_search_active) {
		ui->btn_search->setText(Lang::get(Lang::SearchNext));
	}

	else{
		ui->btn_search->setText(Lang::get(Lang::Search));
	}
}
