/* SomaFMStationView.cpp */

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



#include "SomaFMStationView.h"

SomaFMStationView::SomaFMStationView(QWidget* parent) :
	SearchableTableView(parent)
{}

SomaFMStationView::~SomaFMStationView() {}

int SomaFMStationView::index_by_model_index(const QModelIndex& idx) const
{
	return idx.row();
}

QModelIndex SomaFMStationView::model_index_by_index(int idx) const
{
	return this->model()->index(idx, 0);
}

void SomaFMStationView::keyPressEvent(QKeyEvent *e)
{
	e->setAccepted(false);

	SearchableTableView::keyPressEvent(e);
}
