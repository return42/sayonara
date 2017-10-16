/* ChangeOperations.cpp */

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



#include "ChangeOperations.h"
#include "Components/Library/LibraryManager.h"


ChangeOperation::ChangeOperation()
{
	_library_manager = LibraryManager::instance();
}

ChangeOperation::~ChangeOperation() {}


MoveOperation::MoveOperation(int from, int to) :
	_from(from), _to(to)
{}

void MoveOperation::exec()
{
	_library_manager->move_library(_from, _to);
}

RenameOperation::RenameOperation(int8_t id, const QString& new_name) :
	_id(id),
	_new_name(new_name)
{}

void RenameOperation::exec()
{
	_library_manager->rename_library(_id, _new_name);
}

RemoveOperation::RemoveOperation(int8_t id) :
	_id(id)
{}

void RemoveOperation::exec()
{
	_library_manager->remove_library(_id);
}

AddOperation::AddOperation(const QString& name, const QString& path) :
	_name(name), _path(path)
{}


void AddOperation::exec()
{
	_library_manager->add_library(_name, _path);
}

ChangePathOperation::ChangePathOperation(int8_t id, const QString& new_path) :
	_id(id),
	_new_path(new_path)
{}

void ChangePathOperation::exec()
{
	_library_manager->change_library_path(_id, _new_path);
}
