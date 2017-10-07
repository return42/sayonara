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
	int8_t id;
};

LibraryInfo::LibraryInfo()
{
	m = Pimpl::make<Private>();
	m->id = -1;
}

LibraryInfo::LibraryInfo(const QString& name, const QString& path, int id) :
	LibraryInfo()
{
	m->name = name;
	m->path = Util::File::clean_filename(path);
	m->id = id;
}

LibraryInfo::LibraryInfo(const LibraryInfo& other) :
	LibraryInfo()
{
	m->name = other.name();
	m->path = other.path();
	m->id = other.id();
}

LibraryInfo::~LibraryInfo() {}

LibraryInfo& LibraryInfo::operator =(const LibraryInfo& other)
{
	m->name = other.name();
	m->path = other.path();
	m->id = other.id();

	return *this;
}


QString LibraryInfo::name() const
{
	return m->name;
}

QString LibraryInfo::path() const
{
	return m->path;
}

QString LibraryInfo::symlink_path() const
{
	QString dir = Util::sayonara_path("Libraries");

	QString lib_path = this->path();
	if(lib_path.endsWith("/") || lib_path.endsWith("\\")){
		lib_path.remove(lib_path.size() - 1, 1);
	}

	QString pure_lib_name = Util::File::get_filename_of_path(lib_path);
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

int8_t LibraryInfo::id() const
{
	return m->id;
}

bool LibraryInfo::valid() const
{
	return (!m->name.isEmpty()) && (!m->path.isEmpty());
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
	lst << m->name;
	lst << m->path;
	lst << QString::number(m->id);

	return lst.join("::");
}

bool LibraryInfo::operator==(const LibraryInfo& other) const
{
	return (other.toString() == this->toString());
}
