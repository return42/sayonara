/* LibraryManager.cpp */

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

#include "LibraryManager.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Database/DatabaseConnector.h"
#include "Database/DatabaseLibrary.h"
#include "Utils/Library/LibraryInfo.h"
#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/Settings/Settings.h"
#include "LocalLibrary.h"

#include <QDir>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QString>

using Library::Manager;
using Library::Info;

using OrderMap=QMap<LibraryId, int>;

struct Manager::Private
{
private:
	QMap<LibraryId, LocalLibrary*> lib_map;
	Library::PluginHandler* lph=nullptr;

public:
	QList<Info> all_libs;

	Private()
	{
		lph = Library::PluginHandler::instance();
	}

	bool contains_path(const QString& path) const
	{
		for(const Info& info : all_libs)
		{
			if(path.compare(info.path(), Qt::CaseInsensitive) == 0){
				return true;
			}
		}

		return false;
	}

	LibraryId add_library(const QString& name, const QString& path)
	{
		if(path.isEmpty() || name.isEmpty()){
			return -1;
		}

		if(this->contains_path(path)){
			return -1;
		}

		LibraryId id = this->get_next_id();
		Info info(name, path, id);

		all_libs << info;
		lph->add_local_library(info);

		::Util::File::create_symlink(info.path(), info.symlink_path());

		return id;
	}

	bool rename_library(LibraryId library_id, const QString& name)
	{
		for(int i=0; i<all_libs.size(); i++)
		{
			Info info = all_libs[i];

			if(info.id() != library_id){
				continue;
			}

			if(info.name() == name){
				return false;
			}

			Info new_info(name, info.path(), info.id());

			if(lib_map.contains(library_id))
			{
				LocalLibrary* local_library = lib_map[library_id];
				if(local_library){
					lib_map[library_id]->set_library_name(name);
				}
			}

			QFile::remove(info.symlink_path());
			::Util::File::create_symlink(new_info.path(), new_info.symlink_path());

			all_libs[i] = new_info;

			lph->rename_local_library(library_id, name);

			return true;
		}

		return false;
	}


	void change_library_path(LibraryId library_id, const QString& new_path)
	{
		for(int i=0; i<all_libs.size(); i++)
		{
			Info info = all_libs[i];

			if(info.id() != library_id){
				continue;
			}

			if(info.path() == new_path){
				break;
			}

			Info new_info(info.name(), new_path, info.id());
			all_libs[i] = new_info;

			if(lib_map.contains(info.id()))
			{
				LocalLibrary* library = lib_map[info.id()];
				library->set_library_path(new_path);
			}

			QFile::remove(info.symlink_path());
			::Util::File::create_symlink(new_info.path(), new_info.symlink_path());

			break;
		}
	}

	int get_next_id() const
	{
		LibraryId id=0;
		QList<LibraryId> ids;

		for(const Info& li : all_libs)
		{
			ids << li.id();
		}

		while(ids.contains(id))
		{
			id++;
		}

		return id;
	}

	LocalLibrary* get_library(LibraryId library_id)
	{
		for(const Info& info : all_libs)
		{
			if(info.id() != library_id){
				continue;
			}

			LocalLibrary* lib = nullptr;

			if(lib_map.contains(library_id))
			{
				lib = lib_map[library_id];
				if(lib == nullptr){
					sp_log(Log::Error, this) << "Cannot fetch library";
				}
			}

			else
			{
				lib = new LocalLibrary(library_id, info.name(), info.path());
				lib_map[library_id] = lib;

				if(!lib){
					sp_log(Log::Error, this) << "Cannot create library";
				}
			}

			return lib;
		}

		return nullptr;
	}

	Info get_library_info(LibraryId id)
	{
		for(const Info& info : all_libs)
		{
			if(info.id() == id){
				return info;
			}
		}

		return Info();
	}

	Info get_library_info_by_path(const QString& path)
	{
		Info ret;

		for(const Info& info : all_libs)
		{
			if( path.startsWith(info.path()) &&
				path.length() > ret.path().length())
			{
				ret = info;
			}
		}

		return ret;
	}


	Info get_library_info_by_sympath(const QString& sympath)
	{
		Info ret;

		for(const Info& info : all_libs)
		{
			if( sympath.startsWith(info.symlink_path()) &&
				sympath.length() > ret.symlink_path().length())
			{
				ret = info;
			}
		}

		return ret;
	}

	void move_library(int old_row, int new_row)
	{
		all_libs.move(old_row, new_row);
		lph->move_local_library(old_row, new_row);
	}

	void remove_library(LibraryId id)
	{
		if(lib_map.contains(id))
		{
			LocalLibrary* lib = lib_map[id];
			lib_map.remove(id);

			lib->clear_library();
			delete lib; lib=nullptr;
		}

		for(int i=0; i<all_libs.size(); i++)
		{
			if(all_libs[i].id() == id)
			{
				Library::Info info = all_libs.takeAt(i);
				lph->remove_local_library(id);
				QFile::remove(info.symlink_path());

				return;
			}
		}
	}

	void init_symlinks()
	{

		QString dir = ::Util::sayonara_path("Libraries");
		QDir d(dir);

		QFileInfoList symlinks = d.entryInfoList(QDir::NoFilter);
		for(const QFileInfo& symlink : symlinks)
		{
			if(symlink.isSymLink()) {
				QFile::remove(symlink.absoluteFilePath());
			}
		}

		::Util::File::create_directories(dir);

		for(const Info& info : all_libs)
		{
			QString target = info.symlink_path();

			if(!(QFile::exists(target))){
				::Util::File::create_symlink(info.path(), target);
			}
		}
	}

	OrderMap order_map() const
	{
		OrderMap order_map;
		int i=0;
		for(const ::Library::Info& info : all_libs){
			order_map[info.id()] = i;
			i++;
		}

		return order_map;
	}
};


Manager::Manager() :
	SayonaraClass()
{
	m = Pimpl::make<Private>();

	revert();
	m->init_symlinks();
}

Manager::~Manager() {}

LibraryId Manager::add_library(const QString& name, const QString& path)
{
	LibraryId id = m->add_library(name, path);
	if(id == -1){
		return -1;
	}

	DB::Library* ldb = DB::Connector::instance()->library_connector();

	bool success = ldb->insert_library(id, name, path, 0);
	success = success && ldb->reorder_libraries(m->order_map());

	if(!success){
		return -1;
	}

	return id;
}

bool Manager::rename_library(LibraryId id, const QString& new_name)
{
	m->rename_library(id, new_name);

	Library::Info info = m->get_library_info(id);

	if(info.valid())
	{
		DB::Library* ldb = DB::Connector::instance()->library_connector();
		return ldb->edit_library(id, new_name, info.path());
	}

	sp_log(Log::Warning, this) << "Cannot rename library";
	return false;
}

bool Manager::remove_library(LibraryId id)
{
	m->remove_library(id);

	bool success = true;
	DB::Library* ldb = DB::Connector::instance()->library_connector();
	success = success && ldb->remove_library(id);
	success = success && ldb->reorder_libraries(m->order_map());

	m->all_libs = ldb->get_all_libraries();

	return success;
}

bool Manager::move_library(int old_row, int new_row)
{
	m->move_library(old_row, new_row);

	OrderMap order_map = m->order_map();

	DB::Library* ldb = DB::Connector::instance()->library_connector();

	bool success = ldb->reorder_libraries(order_map);
	m->all_libs = ldb->get_all_libraries();

	return success;
}

bool Manager::change_library_path(LibraryId id, const QString& path)
{
	m->change_library_path(id, path);

	Library::Info info = m->get_library_info(id);
	if(info.valid())
	{
		DB::Library* ldb = DB::Connector::instance()->library_connector();
		return ldb->edit_library(id, info.name(), path);
	}

	else {
		sp_log(Log::Warning, this) << "Cannot change library path";
		return false;
	}
}


QString Manager::request_library_name(const QString& path)
{
	QDir d(path);
	return ::Util::cvt_str_to_first_upper(d.dirName());
}

QList<Info> Manager::all_libraries() const
{
	return m->all_libs;
}

int Manager::count() const
{
	return m->all_libs.size();
}

Info Manager::library_info(LibraryId id) const
{
	return m->get_library_info(id);
}

Library::Info Manager::library_info_by_path(const QString& path) const
{
	return m->get_library_info_by_path(path);
}

Library::Info Manager::library_info_by_sympath(const QString& sympath) const
{
	return m->get_library_info_by_sympath(sympath);
}

LocalLibrary* Manager::library_instance(LibraryId id) const
{
	return m->get_library(id);
}

void Manager::revert()
{
	DB::Library* ldb = DB::Connector::instance()->library_connector();
	m->all_libs = ldb->get_all_libraries();

	if(m->all_libs.isEmpty())
	{
		m->all_libs = _settings->get(Set::Lib_AllLibraries);
		int index = 0;
		for(const Library::Info& info : m->all_libs){
			ldb->insert_library(info.id(), info.name(), info.path(), index);
			index ++;
		}

		_settings->set(Set::Lib_AllLibraries, QList<::Library::Info>());
	}

	if(m->all_libs.isEmpty())
	{
		QString old_path = _settings->get(Set::Lib_Path);

		if(!old_path.isEmpty())
		{
			Info info("Local Library", old_path, 0);
			ldb->insert_library(0, info.name(), info.path(), 0);

			m->all_libs << info;
		}

		_settings->set(Set::Lib_Path, QString());
	}

	for(int i=m->all_libs.size() - 1; i>=0; i--)
	{
		if(!m->all_libs[i].valid()){
			m->all_libs.removeAt(i);
		}
	}
}

