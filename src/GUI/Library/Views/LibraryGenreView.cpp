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
#include "Database/DatabaseHandler.h"
#include "Database/DatabaseConnector.h"

#include "Components/TagEdit/TagEdit.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"

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
	ContextMenu*			context_menu=nullptr;
	GenreNode*				genres=nullptr;
	TreeDelegate*			delegate=nullptr;
	TagEdit*				tag_edit=nullptr;
	QAction*				toggle_tree_action=nullptr;
	QStringList				expanded_items;
	bool					filled;

	Private(QWidget* parent)
	{
		context_menu = new ContextMenu(parent);
		genres = new GenreNode("root");
		delegate = new TreeDelegate(parent);
		tag_edit = new TagEdit(parent);
		filled = false;

		toggle_tree_action = new QAction(context_menu);
		toggle_tree_action->setCheckable(true);
		//toggle_tree_action->setChecked(true);

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

	MetaDataChangeNotifier* mcn = MetaDataChangeNotifier::getInstance();

	this->setAcceptDrops(true);
	this->setDragDropMode(LibraryGenreView::DragDrop);

	this->setAlternatingRowColors(true);
	this->setItemDelegate(_m->delegate);

	connect(this, &QTreeWidget::itemCollapsed, this, &LibraryGenreView::item_collapsed);
	connect(this, &QTreeWidget::itemExpanded, this, &LibraryGenreView::item_expanded);

	connect(mcn, &MetaDataChangeNotifier::sig_metadata_changed, this, &LibraryGenreView::metadata_changed);
	connect(mcn, &MetaDataChangeNotifier::sig_metadata_deleted, this, &LibraryGenreView::metadata_deleted);

	connect(_m->tag_edit, &QThread::finished, this, &LibraryGenreView::update_genre_tags_finished);
	connect(_m->tag_edit, &TagEdit::sig_progress, this, &LibraryGenreView::progress_changed);

	connect( _m->context_menu, &ContextMenu::sig_delete, this, &LibraryGenreView::delete_pressed);
	connect( _m->context_menu, &ContextMenu::sig_rename, this, &LibraryGenreView::rename_pressed);
	connect( _m->context_menu, &ContextMenu::sig_new, this, &LibraryGenreView::new_pressed);

	connect( _m->toggle_tree_action, &QAction::triggered, this, &LibraryGenreView::tree_action_toggled);

	QStringList genres = DatabaseConnector::getInstance()->getAllGenres();
	fill_list(genres);

	REGISTER_LISTENER(Set::Player_Language, language_changed);
}

LibraryGenreView::~LibraryGenreView() {}


QSize LibraryGenreView::sizeHint() const
{
	QSize sz = QTreeView::sizeHint();
	sz.setWidth(200);
	return sz;
}

void LibraryGenreView::reload_genres()
{
	reload_genres(QStringList());
}

void LibraryGenreView::reload_genres(const QStringList& additional_genres)
{
	for(GenreNode* n : _m->genres->children){
		_m->genres->remove_child(n);
		delete n; n=nullptr;
	}

	this->clear();

	// fill it on next show event
	_m->filled = false;

	DatabaseConnector* db = DatabaseConnector::getInstance();
	QStringList genres = db->getAllGenres();
	genres << additional_genres;

	fill_list(genres);
}

int LibraryGenreView::get_row_count() const
{
	int n_rows = _m->genres->children.size();
	return n_rows;
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

	const QMimeData* mime_data;
	const CustomMimeData* cmd;
	MetaDataList v_md;

	QString genre;
	QModelIndex idx;

	this->clearSelection();

	mime_data = e->mimeData();

	cmd = static_cast<const CustomMimeData*>(mime_data);
	if(!cmd){
		sp_log(Log::Debug, this) << "Cannot apply genre to data";
		return;
	}

	idx = this->indexAt(e->pos());
	if(!idx.isValid()){
		sp_log(Log::Debug, this) << "drop: Invalid index";
		return;
	}

	this->setAcceptDrops(false);

	genre = idx.data().toString();
	v_md = cmd->getMetaData();

	_m->tag_edit->set_metadata(v_md);
	for(int i=0; i<v_md.size(); i++){
		_m->tag_edit->add_genre(i, genre);
	}
	_m->tag_edit->commit();

	emit sig_progress(tr("Updating genres"), 0);
}

void LibraryGenreView::contextMenuEvent(QContextMenuEvent* e)
{
	ContextMenu* context_menu = _m->context_menu;
	context_menu->exec(e->globalPos());
	QTreeView::contextMenuEvent(e);
}


void LibraryGenreView::update_genre_tags_finished()
{
	reload_genres();

	emit sig_progress("", -1);

	this->setAcceptDrops(true);
}


void LibraryGenreView::item_expanded(QTreeWidgetItem* item)
{
	_m->expanded_items << item->text(0);
}

void LibraryGenreView::item_collapsed(QTreeWidgetItem* item)
{
	_m->expanded_items.removeAll(item->text(0));
}

void LibraryGenreView::progress_changed(int progress)
{
	emit sig_progress(tr("Updating genres"), progress);
}

void LibraryGenreView::new_pressed()
{
	bool ok;
	QString new_name = QInputDialog::getText(this,
					Lang::get(Lang::Genre),
					Lang::get(Lang::New),
					QLineEdit::Normal, QString(), &ok);

	if(ok && !new_name.isEmpty()){
		QStringList additional_genres;
		additional_genres << new_name;
		reload_genres(additional_genres);
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
			emit sig_rename(text, new_name);
		}

		else{
			return;
		}
	}

	reload_genres();
	this->setCurrentItem( find_genre(new_name) );
}


void LibraryGenreView::delete_pressed()
{
	QList<QTreeWidgetItem*> selected_items = this->selectedItems();
	if(selected_items.isEmpty()){
		return;
	}

	QTreeWidgetItem* item = selected_items.first();
	emit sig_delete(item->text(0));

	reload_genres();
}

void LibraryGenreView::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
	Q_UNUSED(v_md_old)
	Q_UNUSED(v_md_new)

	reload_genres();
}

void LibraryGenreView::metadata_deleted(const MetaDataList& v_md_deleted)
{
	Q_UNUSED(v_md_deleted)

	reload_genres();
}

void LibraryGenreView::tree_action_toggled(bool b)
{
	Q_UNUSED(b)

	reload_genres();
}

void LibraryGenreView::language_changed()
{
	_m->toggle_tree_action->setText(Lang::get(Lang::Tree));
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


void LibraryGenreView::insert_genres(QTreeWidgetItem* parent_item, GenreNode* node)
{
	QTreeWidgetItem* item;
	QStringList text = { Helper::cvt_str_to_first_upper(node->data) };

	if(node->parent == _m->genres){
		item = new QTreeWidgetItem(this, text);
	}

	else{
		item = new QTreeWidgetItem(parent_item, text);
	}

	for(GenreNode* child : node->children){
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
