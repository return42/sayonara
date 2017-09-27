/* LibraryListModel.cpp */

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


#include "LibraryListModel.h"
#include "ChangeOperations.h"
#include "Components/Library/LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Helper/globals.h"

#include <QList>

struct LibraryListModel::Private
{
	QList<LibraryInfo> library_info;
	QList<LibraryInfo> shown_library_info;
	QList<ChangeOperation*> operations;

	Private()
	{
		reload();
	}

	void reload()
	{
		library_info = LibraryManager::getInstance()->all_libraries();
		shown_library_info = library_info;
	}

	void clear_operations()
	{
		for(ChangeOperation* op : operations){
			delete op;
		}

		operations.clear();
	}
};

LibraryListModel::LibraryListModel(QObject* parent) :
	QAbstractListModel(parent)
{
	m = Pimpl::make<Private>();
}

LibraryListModel::~LibraryListModel() {}

int LibraryListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return m->shown_library_info.size();
}

QVariant LibraryListModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();

	if(row < 0 || row >= rowCount()){
		return QVariant();
	}

	if(role == Qt::DisplayRole)	{
		return m->shown_library_info[row].name();
	}

	else if(role == Qt::ToolTipRole) {
		return m->shown_library_info[row].path();
	}

	return QVariant();
}

void LibraryListModel::append_row(const LibName& name, const LibPath& path)
{
	m->operations << new AddOperation(name, path);
	m->shown_library_info << LibraryInfo(name, path, -1);

	emit dataChanged(index(0), index(rowCount()));

}

void LibraryListModel::rename_row(int row, const LibName& new_name)
{
	if(!between(row, m->shown_library_info)) {
		return;
	}

	LibraryInfo info = m->shown_library_info[row];

	m->operations << new RenameOperation(info.id(), new_name);
	m->shown_library_info[row] =
			LibraryInfo(new_name, info.path(), info.id());
}

void LibraryListModel::change_path(int row, const LibPath& path)
{
	if(!between(row, m->shown_library_info)) {
		return;
	}

	LibraryInfo info = m->shown_library_info[row];

	m->operations << new ChangePathOperation(info.id(), path);
	m->shown_library_info[row] =
			LibraryInfo(info.name(), path, info.id());
}

void LibraryListModel::move_row(int from, int to)
{
	if(!between(from, m->shown_library_info)) {
		return;
	}

	if(!between(to, m->shown_library_info)) {
		return;
	}

	m->operations << new MoveOperation(from, to);
	m->shown_library_info.move(from, to);

	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::remove_row(int row)
{
	if(!between(row, m->shown_library_info)) {
		return;
	}

	LibraryInfo info = m->shown_library_info[row];

	m->operations << new RemoveOperation(info.id());
	m->shown_library_info.removeAt(row);

	emit dataChanged(index(0), index(rowCount()));
}

QStringList LibraryListModel::all_names() const
{
	QStringList ret;

	for(const LibraryInfo& info : m->shown_library_info) {
		ret << info.name();
	}

	return ret;
}

QStringList LibraryListModel::all_paths() const
{
	QStringList ret;

	for(const LibraryInfo& info : m->shown_library_info) {
		ret << info.path();
	}

	return ret;
}

void LibraryListModel::reset()
{
	m->reload();
	emit dataChanged(index(0), index(rowCount()));
}

void LibraryListModel::commit()
{
	for(ChangeOperation* op : m->operations){
		op->exec();
	}

	m->reload();
}

