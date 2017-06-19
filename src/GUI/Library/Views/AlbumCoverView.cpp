/* AlbumCoverView.cpp */

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

#include "AlbumCoverView.h"
#include "GUI/Library/Models/AlbumCoverModel.h"
#include "GUI/Library/Delegates/AlbumCoverDelegate.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QHeaderView>
#include <QWheelEvent>
#include <QTimer>

#include <atomic>

struct AlbumCoverView::Private
{
	AlbumCoverModel* model=nullptr;
	QTimer* buffer_timer=nullptr;
	std::atomic<bool> blocked;

	Private()
	{
		blocked = false;
		buffer_timer = new QTimer();
		buffer_timer->setInterval(100);
		buffer_timer->setSingleShot(true);
	}
};

AlbumCoverView::AlbumCoverView(QWidget* parent) :
	LibraryView(parent)
{
	_m = Pimpl::make<Private>();

	set_selection_type( SayonaraSelectionView::SelectionType::Items );
	set_metadata_interpretation(MD::Interpretation::Albums);

	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setSelectionBehavior( QAbstractItemView::SelectItems );
	this->setShowGrid(false);
	this->setItemDelegate(new AlbumCoverDelegate(this));
	this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

	if(horizontalHeader()){
		horizontalHeader()->hide();
	}

	if(verticalHeader()){
		verticalHeader()->hide();
	}

	connect(_m->buffer_timer, &QTimer::timeout, this, &AlbumCoverView::timed_out, Qt::QueuedConnection);
}


AlbumCoverView::~AlbumCoverView() {}


int AlbumCoverView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row() * model()->columnCount() + idx.column();
}

QModelIndex AlbumCoverView::get_model_index_by_index(int idx) const
{
	int row = idx / model()->columnCount();
	int col = idx % model()->columnCount();

	return model()->index(row, col);
}

void AlbumCoverView::setModel(AlbumCoverModel* model)
{
	_m->model = model;

	LibraryView::setModel(_m->model);
	LibraryView::setSearchModel(_m->model);
}

void AlbumCoverView::change_zoom(int zoom)
{
	bool force_reload = (zoom < 0);

	if(force_reload){
		zoom = _m->model->zoom();
	}

	zoom = std::min(zoom, 200);
	zoom = std::max(zoom, 50);

	if(!force_reload){
		if( zoom == _m->model->zoom() ){
			return;
		}
	}

	_m->model->set_zoom(zoom, this->size());
	_settings->set(Set::Lib_CoverZoom, zoom);

	refresh();
}

void AlbumCoverView::refresh()
{
	_m->buffer_timer->start();
}

void AlbumCoverView::timed_out()
{
	if(_m->blocked){
		return;
	}

	_m->blocked = true;

	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	_m->blocked = false;
	_m->buffer_timer->stop();
}


void AlbumCoverView::wheelEvent(QWheelEvent* e)
{
	if( (e->modifiers() & Qt::ControlModifier) &&
		(e->delta() != 0) )
	{
		int zoom;
		if(e->delta() > 0){
			zoom = _m->model->zoom() + 10;
		}

		else {
			zoom = _m->model->zoom() - 10;
		}

		change_zoom(zoom);
	}

	else {
		LibraryView::wheelEvent(e);
	}
}

void AlbumCoverView::resizeEvent(QResizeEvent* e)
{		 
	LibraryView::resizeEvent(e);
	change_zoom();
}

void AlbumCoverView::showEvent(QShowEvent* e)
{
	LibraryView::showEvent(e);
	refresh();
}

QStyleOptionViewItem AlbumCoverView::viewOptions() const
{
	QStyleOptionViewItem option = LibraryView::viewOptions();
	option.decorationAlignment = Qt::AlignHCenter;
	option.displayAlignment = Qt::AlignHCenter;
	option.decorationPosition = QStyleOptionViewItem::Top;

	return option;
}

void AlbumCoverView::setModel(QAbstractItemModel* m)
{
	QTableView::setModel(m);
}

void AlbumCoverView::setModel(LibraryItemModel* m)
{
	LibraryView::setModel(m);
}
