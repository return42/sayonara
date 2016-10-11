/* CustomPlaylistSkeleton.cpp */

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



#include "CustomPlaylistSkeleton.h"

CustomPlaylistSkeleton::CustomPlaylistSkeleton()
{
	_id = -1;
	_num_tracks = 0;
	_is_temporary = false;
}

CustomPlaylistSkeleton::CustomPlaylistSkeleton(const CustomPlaylistSkeleton& s)
{
	_id = s._id;
	_name = s._name;
	_is_temporary = s._is_temporary;
	_num_tracks = s._num_tracks;
}

CustomPlaylistSkeleton::~CustomPlaylistSkeleton(){}

int CustomPlaylistSkeleton::id() const
{ 
	return _id;
}
 
void CustomPlaylistSkeleton::set_id(int id)
{
	_id = id;
}

QString CustomPlaylistSkeleton::name() const
{
	return _name;
}

void CustomPlaylistSkeleton::set_name(const QString& name)
{
	_name = name;
}

bool CustomPlaylistSkeleton::temporary() const
{
	return _is_temporary;
}


void CustomPlaylistSkeleton::set_temporary(bool temporary)
{
	_is_temporary = temporary;
}

int CustomPlaylistSkeleton::num_tracks() const
{
	return _num_tracks;
}

void CustomPlaylistSkeleton::set_num_tracks(int num_tracks)
{
	_num_tracks = num_tracks;
}


