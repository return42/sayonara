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
#include <QDrag>

LibraryView::LibraryView(QWidget* parent) :
	SearchableTableView(parent),
	SayonaraClass(),
	InfoDialogContainer(),
	Dragable(this)
{
	_type = MD::Interpretation::None;
	_cur_filling = false;
	_model = nullptr;

	setAcceptDrops(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	clearSelection();
}


LibraryView::~LibraryView() {}


void LibraryView::setModel(LibraryItemModel * model)
{
	SearchableTableView::setModel(model);
	_model = model;
}


void LibraryView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected )
{
	if(_cur_filling) {
		return;
	}

	SearchableTableView::selectionChanged(selected, deselected);
	SP::Set<int> indexes = get_selected_items();

	emit sig_sel_changed(indexes);
}


void LibraryView::save_selections()
{
	SP::Set<int> indexes = get_selected_items();
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
			get_selected_items()
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
	MetaDataList v_md;

	CustomMimeData* mimedata = _model->get_mimedata();
	if(mimedata){
		v_md = mimedata->getMetaData();
		delete mimedata; mimedata = nullptr;
	}

	return v_md;
}


void LibraryView::rc_menu_show(const QPoint& p) {
	_rc_menu->exec(p);
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


bool LibraryView::event(QEvent* e)
{
	if(e->type() == QEvent::ContextMenu)
	{
		if(!_rc_menu) {
			rc_menu_init();
		}

		SP::Set<int> selections = get_selected_items();

		QContextMenuEvent* cme = dynamic_cast<QContextMenuEvent*>(e);
		QPoint pos = cme->globalPos();

		if(_type == MD::Interpretation::Tracks && selections.size() == 1)
		{
			_rc_menu->show_action(LibraryContextMenu::EntryLyrics, true);
		}
		else{
			_rc_menu->show_action(LibraryContextMenu::EntryLyrics, false);
		}

		bool is_right_type =
			(_type == MD::Interpretation::Artists ||
			_type == MD::Interpretation::Albums);

		if(is_right_type){
			bool has_selections = (selections.size() > 1);

			_merge_action->setVisible(has_selections);

			if(has_selections){
				_merge_menu->clear();

				for(int i : selections)
				{
					QString name = _model->get_string(i);
					name.replace("&", "&&");
					int id = _model->get_id_by_row(i);
					QAction* action = new QAction(name, _merge_menu);
					action->setData(id);

					_merge_menu->addAction(action);
					connect(action, &QAction::triggered, this, &LibraryView::merge_action_triggered);
				}
			}
		}

		rc_menu_show(pos);
	}

	return SearchableTableView::event(e);
}

// mouse events
void LibraryView::mousePressEvent(QMouseEvent* event)
{
	QPoint pos_org = event->pos();
	QPoint pos = QWidget::mapToGlobal(pos_org);

	if(_model->rowCount() == 0){
		return;
	}

	switch(event->button()) {
		case Qt::LeftButton:
			this->drag_pressed(event->pos());
			break;

		case Qt::MidButton:
			emit sig_middle_button_clicked(pos);
			break;

		default:
			break;
	}

	SearchableTableView::mousePressEvent(event);
}


void LibraryView::mouseMoveEvent(QMouseEvent* event)
{
	QDrag* drag = this->drag_moving(event->pos());
	if(drag)
	{
		connect(drag, &QDrag::destroyed, this, [=]()
{
			this->drag_released(Dragable::ReleaseReason::Destroyed);
		});
	}
}

void LibraryView::mouseDoubleClickEvent(QMouseEvent *event)
{
	event->setModifiers(Qt::NoModifier);
	QTableView::mouseDoubleClickEvent(event);
}

void LibraryView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (event->button()) {
		case Qt::LeftButton:

			SearchableTableView::mouseReleaseEvent(event);
			event->accept();

			break;

		default:
			break;
	}
}
// mouse events end

// keyboard events
void LibraryView::keyPressEvent(QKeyEvent* event)
{
	int key = event->key();

	Qt::KeyboardModifiers  modifiers = event->modifiers();

	bool shift_pressed = (modifiers & Qt::ShiftModifier);
	bool alt_pressed = (modifiers & Qt::AltModifier);
	bool ctrl_pressed = (modifiers & Qt::ControlModifier);

	if((key == Qt::Key_Up || key == Qt::Key_Down))
	{
		if(this->selectionModel()->selection().isEmpty())
		{
			if(_model->rowCount() > 0) {
				selectRow(0);
			}

			return;
		}

		if(ctrl_pressed){
			event->setModifiers(Qt::NoModifier);
		}
	}

	SearchableTableView::keyPressEvent(event);
	if(!event->isAccepted()) return;

	SP::Set<int> selections = get_selected_items();

	switch(key) {
		case Qt::Key_Return:
		case Qt::Key_Enter:

			if(selections.isEmpty() || ctrl_pressed){
				break;
			}

			// standard enter
			if(!shift_pressed && !alt_pressed)
			{
				if(!selections.isEmpty()){
					int first_idx = selections.first();
					emit doubleClicked( _model->index(first_idx, 0));
				}
			}

			// enter with shift
			else if(shift_pressed && !alt_pressed) {
				emit sig_append_clicked();
			}

			else if(alt_pressed) {
				emit sig_play_next_clicked();
			}

			break;

		case Qt::Key_End:
			this->selectRow(_model->rowCount() - 1);
			break;

		case Qt::Key_Home:
			this->selectRow(0);
			break;

		default:
			break;
	}
}
// keyboard end


void LibraryView::dragEnterEvent(QDragEnterEvent *event)
{
	event->accept();
}

void  LibraryView::dragMoveEvent(QDragMoveEvent *event)
{
	event->accept();
}

void LibraryView::dropEvent(QDropEvent *event)
{
	event->accept();
	const QMimeData* mime_data = event->mimeData();

	if(!mime_data) return;

	QString text = "";

	if(mime_data->hasText()){
		text = mime_data->text();
	}

	// extern drops
	if( !mime_data->hasUrls() || text.compare("tracks", Qt::CaseInsensitive) == 0) {
		return;
	}

	QStringList filelist;
	for(const QUrl& url : mime_data->urls()) {
		QString path = url.path();

		if(QFile::exists(path)) {
			filelist << path;
		}
	}

	emit sig_import_files(filelist);
}
