/* CustomMimeData.cpp */

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

#include "CustomMimeData.h"
#include "Helper/MetaData/MetaDataList.h"
#include "GUI/Helper/MimeDataHelper.h"

#include <algorithm>

struct CustomMimeData::Private
{
    MetaDataList v_md;
};

CustomMimeData::CustomMimeData() :
	QMimeData()
{
    m = Pimpl::make<Private>();
}

CustomMimeData::~CustomMimeData() {}

void CustomMimeData::set_metadata(const MetaDataList v_md)
{
    m->v_md = v_md;
}

const MetaDataList& CustomMimeData::metadata() const
{
    return m->v_md;
}

bool CustomMimeData::has_metadata() const
{
    return (m->v_md.size() > 0);
}

void CustomMimeData::set_inner_drag_drop()
{
    GUI::MimeData::set_inner_drag_drop(this);
}

bool CustomMimeData::is_inner_drag_drop() const
{
    return GUI::MimeData::is_inner_drag_drop(this);
}
