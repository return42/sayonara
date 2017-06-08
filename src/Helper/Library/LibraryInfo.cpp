/* LibraryInfo.cpp */

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

#include "LibraryInfo.h"
#include "Helper/Helper.h"
#include "Helper/FileHelper.h"

#include <QStringList>
#include <QFile>

struct LibraryInfo::Private
{
	QString path;
	QString name;
	qint8 id;
};

LibraryInfo::LibraryInfo()
{
	_m = Pimpl::make<Private>();
	_m->id = -1;
}

LibraryInfo::LibraryInfo(const QString& name, const QString& path, int id) :
	LibraryInfo()
{
	_m->name = name;
	_m->path = Helper::File::clean_filename(path);
	_m->id = id;
}

LibraryInfo::LibraryInfo(const LibraryInfo& other) :
	LibraryInfo()
{
	_m->name = other.name();
	_m->path = other.path();
	_m->id = other.id();
}

LibraryInfo::~LibraryInfo() {}

LibraryInfo& LibraryInfo::operator =(const LibraryInfo& other)
{
	_m->name = other.name();
	_m->path = other.path();
	_m->id = other.id();

	return *this;
}


QString LibraryInfo::name() const
{
	return _m->name;
}

QString LibraryInfo::path() const
{
	return _m->path;
}

QString LibraryInfo::symlink_path() const
{
	QString dir = Helper::sayonara_path("Libraries");

	QString lib_path = this->path();
	if(lib_path.endsWith("/") || lib_path.endsWith("\\")){
		lib_path.remove(lib_path.size() - 1, 1);
	}

	QString pure_lib_name = Helper::File::get_filename_of_path(lib_path);
	QString lib_name = this->name() + " - " + pure_lib_name;

	lib_name.remove("/");
	lib_name.remove("\\");
	lib_name.remove("#");
	lib_name.remove("#");
	lib_name.remove("*");
	lib_name.remove("{");
	lib_name.remove("}");
	lib_name.remove("?");
	lib_name.remove("\"");
	lib_name.remove("'");

	QString target = dir + "/" + lib_name;

	return target;
}

qint8 LibraryInfo::id() const
{
	return _m->id;
}

bool LibraryInfo::valid() const
{
	return (!_m->name.isEmpty()) && (!_m->path.isEmpty());
}

LibraryInfo LibraryInfo::fromString(const QString& str)
{
	QStringList lst = str.split("::");
	if(lst.size() != 3){
		return LibraryInfo();
	}

	bool ok;
	QString name = lst[0];
	QString path = lst[1];
	int id = lst[2].toInt(&ok);
	if(!ok){
		return LibraryInfo();
	}

	return LibraryInfo(name, path, id);
}

QString LibraryInfo::toString() const
{
	QStringList lst;
	lst << _m->name;
	lst << _m->path;
	lst << QString::number(_m->id);

	return lst.join("::");
}

bool LibraryInfo::operator==(const LibraryInfo& other) const
{
	return (other.toString() == this->toString());
}
