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
#include "Helper/Logger/Logger.h"

#include "GUI/Helper/CustomMimeData.h"
#include "GUI/Helper/ContextMenu/LibraryContextMenu.h"
#include "GUI/Helper/SearchableWidget/MiniSearcher.h"

#include <QHeaderView>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDrag>

struct LibraryView::Private
{
    QAction*			merge_action=nullptr;
    QMenu*				merge_menu=nullptr;
    LibraryContextMenu*	rc_menu=nullptr;

	MD::Interpretation	type;
	bool				cur_filling;

    Private() :
		type(MD::Interpretation::None),
		cur_filling(false)
    {}
};

LibraryView::LibraryView(QWidget* parent) :
    SayonaraWidgetTemplate<SearchableTableView>(parent),
	InfoDialogContainer(),
	Dragable(this)
{
	_model = nullptr;
	m = Pimpl::make<Private>();

	setAcceptDrops(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setAlternatingRowColors(true);
	setDragEnabled(true);

	QHeaderView* vertical_header = verticalHeader();
	if(vertical_header) {
		vertical_header->setResizeContentsPrecision(2);
	}

	clearSelection();
}

LibraryView::~LibraryView() {}

void LibraryView::setModel(LibraryItemModel* model)
{
	SearchableTableView::setModel(model);
	_model = model;
}

void LibraryView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected )
{
	if(m->cur_filling) {
		return;
	}

	SearchableTableView::selectionChanged(selected, deselected);
	SP::Set<int> indexes = get_selected_items();

	emit sig_sel_changed(indexes);
}

// selections end


// Right click stuff
void LibraryView::rc_menu_init()
{
	m->rc_menu = new LibraryContextMenu(this);

	m->rc_menu->show_actions(
				LibraryContextMenu::EntryPlayNext |
				LibraryContextMenu::EntryInfo |
				LibraryContextMenu::EntryDelete |
				LibraryContextMenu::EntryEdit |
				LibraryContextMenu::EntryAppend
	);

	m->merge_menu = new QMenu(tr("Merge"), m->rc_menu);
	m->merge_action = m->rc_menu->addMenu(m->merge_menu);
	m->merge_action->setVisible(false);

	connect(m->rc_menu, &LibraryContextMenu::sig_edit_clicked, this, [=]()
	{show_edit();});
	connect(m->rc_menu, &LibraryContextMenu::sig_info_clicked, this, [=]()
	{show_info();});
	connect(m->rc_menu, &LibraryContextMenu::sig_lyrics_clicked, this, [=]()
	{show_lyrics();});
	connect(m->rc_menu, &LibraryContextMenu::sig_delete_clicked, this, &LibraryView::sig_delete_clicked);
	connect(m->rc_menu, &LibraryContextMenu::sig_play_next_clicked, this, &LibraryView::sig_play_next_clicked);
	connect(m->rc_menu, &LibraryContextMenu::sig_append_clicked, this, &LibraryView::sig_append_clicked);
	connect(m->rc_menu, &LibraryContextMenu::sig_refresh_clicked, this, &LibraryView::sig_refresh_clicked);
}

void LibraryView::show_rc_menu_actions(int entries)
{
	if(!m->rc_menu) {
		rc_menu_init();
	}

	m->rc_menu->show_actions(entries);
}

QMimeData* LibraryView::get_mimedata() const
{
	return _model->get_mimedata();
}

QPixmap LibraryView::pixmap() const
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

void LibraryView::rc_menu_show(const QPoint& p)
{
	m->rc_menu->exec(p);
}


void LibraryView::set_metadata_interpretation(MD::Interpretation type)
{
	m->type = type;
}

MetaDataList LibraryView::info_dialog_data() const
{
    CustomMimeData* cmd = _model->get_mimedata();
    if(!cmd){
        return MetaDataList();
    }

    MetaDataList v_md = cmd->getMetaData();
    delete cmd; cmd = nullptr;

    return v_md;
}


MD::Interpretation LibraryView::metadata_interpretation() const
{
	return m->type;
}

void LibraryView::merge_action_triggered()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	int id = action->data().toInt();

	SP::Set<int> selected_rows = this->get_selected_items();
	SP::Set<ID> ids;
	for(auto it=selected_rows.begin(); it!=selected_rows.end(); it++){
		ids.insert(_model->get_id_by_row(*it));
	}

	emit sig_merge(ids, id);
}

void LibraryView::resize_rows_to_contents()
{
	if(!_model || _model->rowCount() == 0) {
		return;
	}

	QHeaderView* header = this->verticalHeader();
	if(header) {
		header->resizeSections(QHeaderView::ResizeToContents);
	}
}

void LibraryView::resize_rows_to_contents(int first_row, int count)
{
	if(!_model || _model->rowCount() == 0) {
		return;
	}

	QHeaderView* header = this->verticalHeader();
	if(header) {
		for(int i=first_row; i<first_row + count; i++)
		{
			this->resizeRowToContents(i);
		}
	}
}


// mouse events
void LibraryView::mousePressEvent(QMouseEvent* event)
{
    if(_model->rowCount() == 0)
    {
		return;
	}

    QPoint pos_org = event->pos();
    QPoint pos = QWidget::mapToGlobal(pos_org);

    if(event->button() == Qt::LeftButton){
        this->drag_pressed(pos_org);
    }

	SearchableTableView::mousePressEvent(event);

    if(event->button() == Qt::MidButton) {
        // item has to be marked as selected first,
        // so the signal is emmited after calling
        // SearchableTableView::mousePressEvent(event);
        emit sig_middle_button_clicked(pos);
    }
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
	if(is_minisearcher_active()) {
		return;
	}

	SP::Set<int> selections = get_selected_items();

	switch(key)
	{
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

void LibraryView::contextMenuEvent(QContextMenuEvent* event)
{
    if(!m->rc_menu)
    {
		rc_menu_init();
	}

	SP::Set<int> selections = get_selected_items();

	QPoint pos = event->globalPos();

	if(m->type == MD::Interpretation::Tracks && selections.size() == 1)
	{
		m->rc_menu->show_action(LibraryContextMenu::EntryLyrics, true);
	}
    else {
		m->rc_menu->show_action(LibraryContextMenu::EntryLyrics, false);
	}

	bool is_right_type =
			(m->type == MD::Interpretation::Artists ||
			 m->type == MD::Interpretation::Albums);

	if(is_right_type) {
		size_t n_selections = selections.size();

		if(n_selections > 1){
			m->merge_menu->clear();

			for(int i : selections)
			{
				int id = _model->get_id_by_row(i);
				if(id < 0){
					n_selections--;
					if(n_selections <= 1) {
						break;
					}

					continue;
				}

				QString name = _model->get_string(i);
				name.replace("&", "&&");

				QAction* action = new QAction(name, m->merge_menu);
				action->setData(id);
				m->merge_menu->addAction(action);
				connect(action, &QAction::triggered, this, &LibraryView::merge_action_triggered);
			}

			m->merge_action->setVisible(n_selections > 1);
		}
	}

	rc_menu_show(pos);

	QTableView::contextMenuEvent(event);
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
	if(!mime_data) {
		return;
	}

	QString text;

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

void LibraryView::changeEvent(QEvent* event)
{
	SearchableTableView::changeEvent(event);
	QEvent::Type type = event->type();

	if(type == QEvent::FontChange)
	{
		resize_rows_to_contents();
	}
}
