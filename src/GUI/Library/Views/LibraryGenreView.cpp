/* LibraryGenreView.cpp */

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

#include "LibraryGenreView.h"

#include "Components/Library/GenreFetcher.h"

#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/Delegates/TreeDelegate.h"
#include "GUI/Helper/ContextMenu/ContextMenu.h"

#include "Helper/Helper.h"
#include "Helper/Tree.h"
#include "Helper/Set.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Language.h"
#include "Helper/Settings/Settings.h"

#include <QDropEvent>
#include <QContextMenuEvent>
#include <QInputDialog>

typedef SP::Set<QString> StringSet;

struct LibraryGenreView::Private
{
	GenreFetcher*			genre_fetcher=nullptr;
	ContextMenu*			context_menu=nullptr;
	GenreNode*				genres=nullptr;
	QAction*				toggle_tree_action=nullptr;
	QStringList				expanded_items;
	bool					filled;
	int						default_indent;

	Private(QWidget* parent)
	{
		genre_fetcher = new GenreFetcher(parent);
		context_menu = new ContextMenu(parent);
		genres = new GenreNode("root");
		filled = false;

		toggle_tree_action = new QAction(context_menu);
		toggle_tree_action->setCheckable(true);

		context_menu->show_actions(
					ContextMenu::EntryDelete |
					ContextMenu::EntryNew |
					ContextMenu::EntryRename );

		context_menu->register_action(toggle_tree_action);
	}

	~Private()
	{
		delete genres; genres=nullptr;
	}
};

LibraryGenreView::LibraryGenreView(QWidget* parent) :
	SayonaraWidgetTemplate<QTreeWidget>(parent)
{
	_m = Pimpl::make<Private>(this);
	_m->default_indent = this->indentation();

	bool show_tree = _settings->get(Set::Lib_GenreTree);

	this->setAcceptDrops(true);
	this->setDragDropMode(LibraryGenreView::DragDrop);
	this->setAlternatingRowColors(true);
	this->setItemDelegate(new TreeDelegate(this));
	_m->toggle_tree_action->setChecked(show_tree);

	connect(this, &QTreeWidget::itemCollapsed, this, &LibraryGenreView::item_collapsed);
	connect(this, &QTreeWidget::itemExpanded, this, &LibraryGenreView::item_expanded);

	connect(_m->genre_fetcher, &GenreFetcher::sig_finished, this, &LibraryGenreView::update_finished);
	connect(_m->genre_fetcher, &GenreFetcher::sig_progress, this, &LibraryGenreView::progress_changed);
	connect(_m->genre_fetcher, &GenreFetcher::sig_genres_fetched, this, &LibraryGenreView::reload_genres);

	connect( _m->context_menu, &ContextMenu::sig_delete, this, &LibraryGenreView::delete_pressed);
	connect( _m->context_menu, &ContextMenu::sig_rename, this, &LibraryGenreView::rename_pressed);
	connect( _m->context_menu, &ContextMenu::sig_new, this, &LibraryGenreView::new_pressed);

	connect( _m->toggle_tree_action, &QAction::triggered, this, &LibraryGenreView::tree_action_toggled);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
	REGISTER_LISTENER_NO_CALL(Set::Lib_GenreTree, tree_action_changed);
}

LibraryGenreView::~LibraryGenreView() {}

QSize LibraryGenreView::sizeHint() const
{
	QSize sz = QTreeView::sizeHint();
	sz.setWidth(200);
	return sz;
}

int LibraryGenreView::row_count() const
{
	int n_rows = _m->genres->children.size();
	if(n_rows == 1){
		QString data = _m->genres->children[0]->data;
		if(data.trimmed().isEmpty()){
			return 0;
		}
	}

	return n_rows;
}

void LibraryGenreView::set_local_library(LocalLibrary* library)
{
	_m->genre_fetcher->set_local_library(library);
	_m->genre_fetcher->reload_genres();
}

void LibraryGenreView::progress_changed(int progress)
{
	emit sig_progress(tr("Updating genres"), progress);
}

void LibraryGenreView::update_finished()
{
	emit sig_progress("", -1);
}

void LibraryGenreView::reload_genres()
{
	QStringList genres = _m->genre_fetcher->genres();
	for(GenreNode* n : _m->genres->children){
		_m->genres->remove_child(n);
		delete n; n=nullptr;
	}

	this->clear();

	// fill it on next show event
	_m->filled = false;

	fill_list(genres);

	emit sig_genres_reloaded();
}

void LibraryGenreView::fill_list(const QStringList& genres)
{
	if(_m->filled){
		return;
	}

	_m->filled = true;

	this->init_data(genres);
	this->insert_genres(nullptr, _m->genres);
}

void LibraryGenreView::item_expanded(QTreeWidgetItem* item)
{
	_m->expanded_items << item->text(0);
}

void LibraryGenreView::item_collapsed(QTreeWidgetItem* item)
{
	_m->expanded_items.removeAll(item->text(0));
}

void LibraryGenreView::new_pressed()
{
	bool ok;
	QString new_name = QInputDialog::getText(this,
					Lang::get(Lang::Genre),
					Lang::get(Lang::New),
					QLineEdit::Normal, QString(), &ok);

	if(ok && !new_name.isEmpty()){
		_m->genre_fetcher->create_genre(new_name);
	}
}

void LibraryGenreView::rename_pressed()
{
	QList<QTreeWidgetItem*> selected_items = this->selectedItems();
	if(selected_items.isEmpty()){
		return;
	}

	bool ok;
	QString new_name;
	for(const QTreeWidgetItem* item : selected_items) {
		QString text = item->text(0);

		new_name = QInputDialog::getText(this,
						Lang::get(Lang::Genre),
						Lang::get(Lang::Rename) + " " + text + ": ",
						QLineEdit::Normal, QString(), &ok);
		if(ok && !new_name.isEmpty()){
			_m->genre_fetcher->rename_genre(text, new_name);
		}
	}
}


void LibraryGenreView::delete_pressed()
{
	QList<QTreeWidgetItem*> selected_items = this->selectedItems();
	if(selected_items.isEmpty()){
		return;
	}

	QTreeWidgetItem* item = selected_items.first();
	_m->genre_fetcher->delete_genre(item->text(0));
}

void LibraryGenreView::tree_action_changed()
{
	bool show_tree = _settings->get(Set::Lib_GenreTree);
	reload_genres();

	if(!show_tree) {
		this->setIndentation(0);
	}

	else {
		this->setIndentation(_m->default_indent);
	}
}

void LibraryGenreView::tree_action_toggled(bool b)
{
	_settings->set(Set::Lib_GenreTree, b);
}

void LibraryGenreView::language_changed()
{
	_m->toggle_tree_action->setText(Lang::get(Lang::Tree));
}

void LibraryGenreView::insert_genres(QTreeWidgetItem* parent_item, GenreNode* node)
{
	QTreeWidgetItem* item;
	QStringList text = { Helper::cvt_str_to_first_upper(node->data) };

	if(node->parent == _m->genres){
		item = new QTreeWidgetItem(this, text);
	}

	else {
		item = new QTreeWidgetItem(parent_item, text);
	}

	for(GenreNode* child : node->children) {
		insert_genres(item, child);
	}

	if(_m->expanded_items.contains(node->data, Qt::CaseInsensitive)){
		item->setExpanded(true);
	}
}

QTreeWidgetItem* LibraryGenreView::find_genre(const QString& genre)
{
	QList<QTreeWidgetItem*> items = this->findItems(genre, Qt::MatchRecursive);

	if(items.isEmpty()){
		sp_log(Log::Warning, this) << "Could not find item " << genre;
		return nullptr;
	}

	return items.first();
}


static void build_genre_node(GenreNode* node, const QMap<QString, StringSet>& parent_nodes)
{
	QString value = node->data;
	if(!parent_nodes.contains(value)){
		return;
	}

	const StringSet& children = parent_nodes[value];
	if(children.isEmpty()){
		return;
	}

	for(const QString& str : children){
		GenreNode* new_child = new GenreNode(str);
		build_genre_node( new_child, parent_nodes );
		node->add_child(new_child);
	}
}


void LibraryGenreView::init_data(const QStringList& genres)
{
	SP::Set<QString> genre_set;
	if(_m->genres){
		delete _m->genres;
	}

	_m->genres = new GenreNode("");
	QMap<QString, StringSet> children;

	for(const QString& s : genres) {
		genre_set.insert(s);
	}

	for(auto it=genre_set.begin(); it != genre_set.end(); it++) 
	{
		bool found_parent = false;
		Genre genre(*it);
		if(genre.name().isEmpty()){
			continue;
		}

		if(_m->toggle_tree_action->isChecked())
		{
			for(auto subit=genre_set.begin(); subit != genre_set.end(); subit++)
			{
				Genre parent_genre(*subit);

				if( parent_genre.name().isEmpty() ||
					parent_genre == genre)
				{
					continue;
				}

				if( genre.name().contains(parent_genre.name(), Qt::CaseInsensitive) ) {
					StringSet& child_genres = children[parent_genre.name()];
					child_genres.insert(genre.name());
					found_parent = true;
				}
			}
		}

		if(!found_parent) {
			_m->genres->add_child(genre.name());
		}
	}

	for(GenreNode* base_genre : _m->genres->children){
		build_genre_node(base_genre, children);
	}

	_m->genres->sort(true);
}



void LibraryGenreView::keyPressEvent(QKeyEvent* e)
{
	if( e->key() == Qt::Key_Enter ||
		e->key() == Qt::Key_Return)
	{
		QTreeWidgetItem* item = this->currentItem();
		item->setExpanded(true);
	}

	QTreeWidget::keyPressEvent(e);
}

void LibraryGenreView::contextMenuEvent(QContextMenuEvent* e)
{
	ContextMenu* context_menu = _m->context_menu;
	context_menu->exec(e->globalPos());
	QTreeView::contextMenuEvent(e);
}


void LibraryGenreView::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept();
}

void LibraryGenreView::dragMoveEvent(QDragMoveEvent *e)
{
	QItemSelectionModel* ism;
	QModelIndex idx;

	idx = this->indexAt(e->pos());
	if(!idx.isValid()){
		sp_log(Log::Debug, this) << "drag: Invalid index";
		return;
	}

	ism = this->selectionModel();

	ism->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

	e->accept();
}

void LibraryGenreView::dragLeaveEvent(QDragLeaveEvent *e)
{
	this->clearSelection();
	e->accept();
}

void LibraryGenreView::dropEvent(QDropEvent *e)
{
	e->accept();

	this->clearSelection();

	const CustomMimeData* cmd = static_cast<const CustomMimeData*>(e->mimeData());

	if(!cmd){
		sp_log(Log::Debug, this) << "Cannot apply genre to data";
		return;
	}

	QModelIndex idx = this->indexAt(e->pos());
	if(!idx.isValid()){
		sp_log(Log::Debug, this) << "drop: Invalid index";
		return;
	}

	this->setAcceptDrops(false);

	QString genre = idx.data().toString();
	MetaDataList v_md = cmd->getMetaData();

	_m->genre_fetcher->add_genre_to_md(v_md, genre);

}
