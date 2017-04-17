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
#include "Models/AlbumCoverModel.h"
#include "GUI/Library/Models/AlbumCoverModel.h"
#include "GUI/Library/Delegates/AlbumCoverDelegate.h"
#include "Helper/Logger/Logger.h"

#include <QHeaderView>
#include <QWheelEvent>
#include <QTimer>
#include <atomic>

struct AlbumCoverView::Private
{
	int zoom;
	AlbumCoverModel* model=nullptr;
	QTimer* buffer_timer=nullptr;
	std::atomic<int> needs_refresh;

	Private()
	{
		zoom = 100;
		buffer_timer = new QTimer();
		buffer_timer->setInterval(500);
		needs_refresh = 0;
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

	connect(_m->buffer_timer, &QTimer::timeout, this, [=](){
		if(_m->needs_refresh > 0){

			this->resizeRowsToContents();

			// this->resizeColumnsToContents();
			// Todo: Workaround. maybe player won't crash anymore
			for(int i=0; i<_m->model->columnCount(); i++){
				this->resizeColumnToContents(i);
			}

			_m->needs_refresh = 0;
			if(_m->needs_refresh == 0){
				_m->buffer_timer->stop();
				sp_log(Log::Debug, this) << " Kill timer";
			}

			if(_m->needs_refresh < 0){
				sp_log(Log::Warning, this) << " Timer too small";
			}
		}
	});
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
	if(zoom != -1){
		zoom = std::min(zoom, 200);
		zoom = std::max(zoom, 50);

		if( zoom == _m->zoom ){
			return;
		}
	}

	else{
		zoom = _m->zoom;
	}

	_m->zoom = zoom;
	_m->model->set_zoom(_m->zoom);

	int col_width = _m->model->get_item_size().width();
	int n_cols_target = (this->width() + (col_width / 10)) / col_width;

	_m->model->set_max_columns(n_cols_target);
	_m->needs_refresh++;

	if(_m->needs_refresh == 1){
		sp_log(Log::Debug, this) << " Start timer";
		_m->buffer_timer->start();
	}
}

void AlbumCoverView::refresh()
{
	_m->needs_refresh ++;
	if(_m->needs_refresh == 1){
		sp_log(Log::Debug, this) << " Start timer";
		_m->buffer_timer->start();
	}
}


void AlbumCoverView::wheelEvent(QWheelEvent* e)
{
	if( (e->modifiers() & Qt::ControlModifier) &&
		(e->delta() != 0) )
	{
		int zoom;
		if(e->delta() > 0){
			zoom = _m->zoom + 10;
		}

		else {
			zoom = _m->zoom - 10;
		}

		change_zoom(zoom);
	}

	else{
		LibraryView::wheelEvent(e);
	}
}

void AlbumCoverView::resizeEvent(QResizeEvent* e)
{		 
	LibraryView::resizeEvent(e);
	change_zoom();
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
