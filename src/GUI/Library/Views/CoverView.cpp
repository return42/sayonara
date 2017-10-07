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

#include "CoverView.h"
#include "GUI/Library/Models/CoverModel.h"
#include "GUI/Library/Delegates/CoverDelegate.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Logger/Logger.h"

#include <QHeaderView>
#include <QWheelEvent>
#include <QTimer>

#include <atomic>

using namespace Library;

struct CoverView::Private
{
	CoverModel* model=nullptr;
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

CoverView::CoverView(QWidget* parent) :
	View(parent)
{
	m = Pimpl::make<Private>();

	set_selection_type( SayonaraSelectionView::SelectionType::Items );
	set_metadata_interpretation(MD::Interpretation::Albums);

	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setSelectionBehavior( QAbstractItemView::SelectItems );
	this->setShowGrid(false);
	this->setItemDelegate(new CoverDelegate(this));
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

	if(horizontalHeader()){
		horizontalHeader()->hide();
	}

	if(verticalHeader()){
		verticalHeader()->hide();
	}

	connect(m->buffer_timer, &QTimer::timeout, this, &CoverView::timed_out, Qt::QueuedConnection);
}


CoverView::~CoverView() {}


int CoverView::get_index_by_model_index(const QModelIndex& idx) const
{
	return idx.row() * model()->columnCount() + idx.column();
}

QModelIndex CoverView::get_model_index_by_index(int idx) const
{
	int row = idx / model()->columnCount();
	int col = idx % model()->columnCount();

	return model()->index(row, col);
}

void CoverView::setModel(CoverModel* model)
{
	m->model = model;

	View::setModel(m->model);
	View::setSearchModel(m->model);
}

void CoverView::change_zoom(int zoom)
{
	bool force_reload = (zoom < 0);

	if(force_reload){
		zoom = m->model->zoom();
	}

	zoom = std::min(zoom, 200);
	zoom = std::max(zoom, 50);

	if(!force_reload){
		if( zoom == m->model->zoom() ){
			return;
		}
	}

	m->model->set_zoom(zoom, this->size());
	_settings->set(Set::Lib_CoverZoom, zoom);

	refresh();
}

void CoverView::refresh()
{
	m->buffer_timer->start();
}

void CoverView::timed_out()
{
	if(m->blocked){
		return;
	}

	m->blocked = true;

	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	m->blocked = false;
	m->buffer_timer->stop();
}


void CoverView::wheelEvent(QWheelEvent* e)
{
	if( (e->modifiers() & Qt::ControlModifier) &&
		(e->delta() != 0) )
	{
		int zoom;
		if(e->delta() > 0){
			zoom = m->model->zoom() + 10;
		}

		else {
			zoom = m->model->zoom() - 10;
		}

		change_zoom(zoom);
	}

	else {
		View::wheelEvent(e);
	}
}

void CoverView::resizeEvent(QResizeEvent* e)
{		 
	View::resizeEvent(e);
	change_zoom();
}

void CoverView::showEvent(QShowEvent* e)
{
	View::showEvent(e);
	refresh();
}

QStyleOptionViewItem CoverView::viewOptions() const
{
	QStyleOptionViewItem option = View::viewOptions();
	option.decorationAlignment = Qt::AlignHCenter;
	option.displayAlignment = Qt::AlignHCenter;
	option.decorationPosition = QStyleOptionViewItem::Top;

	return option;
}

void CoverView::setModel(QAbstractItemModel* m)
{
	QTableView::setModel(m);
}

void CoverView::setModel(ItemModel* m)
{
	View::setModel(m);
}
