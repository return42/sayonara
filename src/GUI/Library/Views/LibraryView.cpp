/* LibraryView.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
 * LibraryView.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: Lucio Carreras
 */

#include "LibraryView.h"
#include "HeaderView.h"
#include "Components/Covers/CoverLocation.h"
#include "GUI/Library/Helper/ColumnHeader.h"

#include "Helper/globals.h"
#include "Helper/Settings/Settings.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/Set.h"

#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/SearchableWidget/MiniSearcher.h"

#include <QDropEvent>
#include <QMouseEvent>

LibraryView::LibraryView(QWidget* parent) :
	SearchableTableView(parent),
	SayonaraClass(),
	InfoDialogContainer(),
	Dragable(this)
{
	_type = MD::Interpretation::None;
	_cur_filling = false;
	_model = nullptr;

	HeaderView* header = new HeaderView(Qt::Horizontal, this);
	this->setHorizontalHeader(header);

	connect(header, &HeaderView::sectionClicked, this, &LibraryView::sort_by_column);
	connect(header, &HeaderView::sig_columns_changed, this, &LibraryView::header_actions_triggered);

	setAcceptDrops(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	clearSelection();

	REGISTER_LISTENER_NO_CALL(Set::Player_Language, language_changed);
}


LibraryView::~LibraryView() {}


void LibraryView::setModel(LibraryItemModel * model)
{
	SearchableTableView::setModel(model);
	_model = model;

	language_changed();
}


void LibraryView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected )
{
	if(_cur_filling) {
		return;
	}

	SearchableTableView::selectionChanged(selected, deselected);
	SP::Set<int> indexes = get_selections(_selection_type);

	emit sig_sel_changed(indexes);
}


void LibraryView::save_selections()
{
	SP::Set<int> indexes = get_selections(_selection_type);
	_model->add_selections(indexes);
}
// selections end


// Right click stuff
void LibraryView::rc_menu_init()
{
	_rc_menu = new LibraryContextMenu(this);

	_rc_menu->show_actions(
				LibraryContextMenu::EntryPlayNext |
				LibraryContextMenu::EntryInfo |
				LibraryContextMenu::EntryDelete |
				LibraryContextMenu::EntryEdit |
				LibraryContextMenu::EntryAppend
	);

	_merge_menu = new QMenu(tr("Merge"), _rc_menu);
	_merge_action = _rc_menu->addMenu(_merge_menu);
	_merge_action->setVisible(false);

	connect(_rc_menu, &LibraryContextMenu::sig_edit_clicked, this, [=]()
{show_edit();});
	connect(_rc_menu, &LibraryContextMenu::sig_info_clicked, this, [=]()
{show_info();});
	connect(_rc_menu, &LibraryContextMenu::sig_lyrics_clicked, this, [=]()
{show_lyrics();});
	connect(_rc_menu, &LibraryContextMenu::sig_delete_clicked, this, &LibraryView::sig_delete_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_play_next_clicked, this, &LibraryView::sig_play_next_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_append_clicked, this, &LibraryView::sig_append_clicked);
	connect(_rc_menu, &LibraryContextMenu::sig_refresh_clicked, this, &LibraryView::sig_refresh_clicked);
}

void LibraryView::show_rc_menu_actions(int entries)
{
	if(!_rc_menu){
		rc_menu_init();
	}

	_rc_menu->show_actions(entries);
}

QMimeData* LibraryView::get_mimedata() const
{
	return _model->get_mimedata();
}

QPixmap LibraryView::get_pixmap() const
{
	CoverLocation cl = _model->get_cover(
			get_selections(_selection_type)
	);

	QString cover_path = cl.preferred_path();
	if(cl.valid()){
		return QPixmap(cover_path);
	}

	return QPixmap();
}

void LibraryView::set_selection_type(SayonaraSelectionView::SelectionType type)
{
	SayonaraSelectionView::set_selection_type(type);

	if(type == SayonaraSelectionView::SelectionType::Rows){
		setSelectionBehavior(QAbstractItemView::SelectRows);
	}

	else {
		setSelectionBehavior(QAbstractItemView::SelectColumns);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	}
}

MetaDataList LibraryView::get_selected_metadata() const
{
	CustomMimeData* mimedata;
	MetaDataList v_md;

	mimedata = _model->get_mimedata();
	if(mimedata){
		v_md = mimedata->getMetaData();
		delete mimedata; mimedata = nullptr;
	}

	return v_md;
}


void LibraryView::rc_menu_show(const QPoint& p) {
	_rc_menu->exec(p);
}


void LibraryView::language_changed()
{
	HeaderView* header_view = get_header_view();

	for(int i=0; i<_model->columnCount(); i++){
		ColumnHeader* header = header_view->get_column_header(i);
		if(header){
			_model->setHeaderData(i, Qt::Horizontal, header->get_title(), Qt::DisplayRole);
		}
	}
}

void LibraryView::set_type(MD::Interpretation type)
{
	_type = type;
}

MetaDataList LibraryView::get_data_for_info_dialog() const
{
	return get_selected_metadata();
}


MD::Interpretation LibraryView::get_metadata_interpretation() const
{
	return _type;
}

void LibraryView::merge_action_triggered()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	int id = action->data().toInt();

	emit sig_merge(id);
}
