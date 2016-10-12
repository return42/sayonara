/* CustomMimeData.cpp */

/* Copyright (C) 2011-2016  Lucio Carreras
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

struct CustomMimeData::Private
{
    MetaDataList    v_md;
	QString			cover_url;
};

CustomMimeData::CustomMimeData() :
	QMimeData()
{
	_m = new CustomMimeData::Private();
}

CustomMimeData::~CustomMimeData()
{
	delete _m; _m = nullptr;
}


void CustomMimeData::setMetaData(const MetaDataList& v_md)
{
    _m->v_md = v_md;
}

MetaDataList CustomMimeData::getMetaData() const
{
	return _m->v_md;
}

bool CustomMimeData::hasMetaData() const
{
	return (_m->v_md.size() > 0);
}

// TODO: Never used
/*void CustomMimeData::setCoverUrl(const QString& url)
{
	_m->cover_url = url;
}*/

QString CustomMimeData::getCoverUrl() const 
{
	return _m->cover_url;
}

bool CustomMimeData::hasCoverUrl() const
{
	return (!_m->cover_url.isEmpty());
}
