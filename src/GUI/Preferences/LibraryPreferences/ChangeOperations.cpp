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

ChangeOperation::ChangeOperation() {}

ChangeOperation::~ChangeOperation() {}

Library::Manager* ChangeOperation::manager() const
{
	return Library::Manager::instance();
}

struct MoveOperation::Private
{
	int from, to;
	Private(int from, int to) :
		from(from),
		to(to)
	{}
};

MoveOperation::MoveOperation(int from, int to)
{
	m = Pimpl::make<Private>(from, to);
}

MoveOperation::~MoveOperation() {}

bool MoveOperation::exec()
{
	return manager()->move_library(m->from, m->to);
}


struct RenameOperation::Private
{
	int8_t id;
	QString new_name;

	Private(int8_t id, const QString& new_name) :
		id(id),
		new_name(new_name)
	{}
};

RenameOperation::RenameOperation(int8_t id, const QString& new_name)
{
	m = Pimpl::make<Private>(id, new_name);
}

RenameOperation::~RenameOperation() {}

bool RenameOperation::exec()
{
	return manager()->rename_library(m->id, m->new_name);
}

struct RemoveOperation::Private
{
	int8_t id;

	Private(int8_t id) :
		id(id)
	{}
};

RemoveOperation::RemoveOperation(int8_t id)
{
	m = Pimpl::make<Private>(id);
}

RemoveOperation::~RemoveOperation() {}

bool RemoveOperation::exec()
{
	return manager()->remove_library(m->id);
}

struct AddOperation::Private
{
	QString name, path;

	Private(const QString& name, const QString& path) :
		name(name),
		path(path)
	{}
};

AddOperation::AddOperation(const QString& name, const QString& path)
{
	m = Pimpl::make<Private>(name, path);
}

AddOperation::~AddOperation() {}

bool AddOperation::exec()
{
	return (manager()->add_library(m->name, m->path) >= 0);
}

struct ChangePathOperation::Private
{
	int8_t id;
	QString new_path;

	Private(int8_t id, const QString& new_path) :
		id(id),
		new_path(new_path)
	{}
};

ChangePathOperation::ChangePathOperation(int8_t id, const QString& new_path)
{
	m = Pimpl::make<Private>(id, new_path);
}

ChangePathOperation::~ChangePathOperation() {}

bool ChangePathOperation::exec()
{
	return manager()->change_library_path(m->id, m->new_path);
}
