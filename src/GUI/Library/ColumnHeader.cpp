
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


#include "ColumnHeader.h"


ColumnHeader::ColumnHeader(QString title, bool switchable, SortOrder sort_asc, SortOrder sort_desc, int preferred_size_abs){

	_title = title;
	_switchable = switchable;
	_abs_size = true;
	_sort_asc = sort_asc;
	_sort_desc = sort_desc;
	_preferred_size_abs = preferred_size_abs;
	_preferred_size_rel = 0;
}

ColumnHeader::ColumnHeader(QString title, bool switchable, SortOrder sort_asc, SortOrder sort_desc, double preferred_size_rel, int min_size) :
	ColumnHeader(title, switchable, sort_asc, sort_desc, 0)

{
	_abs_size = false;
	_preferred_size_abs = min_size;
	_preferred_size_rel = preferred_size_rel;
}

void ColumnHeader::set_preferred_size_abs(int preferred_size){
	_preferred_size_abs = preferred_size;
	_preferred_size_rel = 0;
	_abs_size = true;
}

void ColumnHeader::set_preferred_size_rel(double preferred_size){
	_preferred_size_rel = preferred_size;
	_preferred_size_abs = 0;
	_abs_size = false;
}

QString ColumnHeader::getTitle() const {
	return _title;
}

bool ColumnHeader::getSwitchable() const {
	return _switchable;
}

ColHeaderSize ColumnHeader::getSizeType() const {
	return (_abs_size ? ColHeaderSizeAbs : ColHeaderSizeRel);
}

int ColumnHeader::get_preferred_size_abs() const {
	return _preferred_size_abs;
}

double ColumnHeader::get_preferred_size_rel() const {
	return _preferred_size_rel;
}

SortOrder ColumnHeader::get_asc_sortorder() const {
	return _sort_asc;
}

SortOrder ColumnHeader::get_desc_sortorder() const {
	return _sort_desc;
}
