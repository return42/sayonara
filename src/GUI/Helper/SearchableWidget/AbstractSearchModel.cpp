/* AbstractSearchModel.cpp */

/* Copyright (C) 2011-2015  Lucio Carreras
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



#include "AbstractSearchModel.h"

AbstractSearchModelInterface::AbstractSearchModelInterface(){}
AbstractSearchModelInterface::~AbstractSearchModelInterface(){}

void AbstractSearchModelInterface::set_search_mode(Library::SearchModeMask search_mode)
{
	_search_mode = search_mode;
}

Library::SearchModeMask AbstractSearchModelInterface::search_mode() const
{
	return _search_mode;
}

AbstractSearchTableModel::AbstractSearchTableModel(QObject* parent) :
    QAbstractTableModel(parent),
    AbstractSearchModelInterface()
{}

AbstractSearchTableModel::~AbstractSearchTableModel(){}

AbstractSearchListModel::AbstractSearchListModel(QObject* parent) :
    QAbstractListModel(parent),
    AbstractSearchModelInterface()
{}

AbstractSearchListModel::~AbstractSearchListModel(){}
