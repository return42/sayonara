#include "LibraryManager.h"
#include "Interfaces/LibraryInterface/LibraryPluginHandler.h"
#include "Helper/Library/LibraryInfo.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "LocalLibrary.h"

#include <QMap>
#include <QObject>
#include <QString>


struct LibraryManager::Private
{
	QList<LibraryInfo> all_libs;
	QMap<qint8, LocalLibrary*> lib_map;
	LibraryPluginHandler* lph=nullptr;

	Private()
	{
		lph = LibraryPluginHandler::getInstance();
	}

	bool contains_path(const QString& path) const
	{
		for(const LibraryInfo& info : all_libs){
			//QString info_path = info.path();
			if(path.compare(info.path(), Qt::CaseInsensitive) == 0){
				return true;
			}
		}

		return false;
	}

	void rename_library(qint8 library_id, const QString& name)
	{
		for(int i=0; i<all_libs.size(); i++)
		{
			qint8 id = all_libs[i].id();
			QString path = all_libs[i].path();
			if(id == library_id){
				all_libs[i] = LibraryInfo(name, path, id);
				break;
			}
		}
	}


	void set_library_path(qint8 library_id, const QString& library_path)
	{
		LibraryInfo new_info;
		for(auto it=all_libs.begin(); it!=all_libs.end(); it++)
		{
			if(it->id() == library_id){
				new_info = LibraryInfo(it->name(), library_path, it->id());
				all_libs.erase(it);
				break;
			}
		}

		if(new_info.valid()){
			all_libs << new_info;
		}
	}

	int get_next_id() const
	{
		qint8 id=0;
		QList<qint8> ids;
		for(const LibraryInfo& li : all_libs){
			ids << li.id();
		}

		while(ids.contains(id)){
			id++;
		}

		return id;
	}

	LocalLibrary* get_library(qint8 library_id)
	{
		for(const LibraryInfo& li : all_libs)
		{
			if(li.id() == library_id){
				if(lib_map.contains(library_id)){
					return lib_map[library_id];
				}

				else {
					LocalLibrary* lib = new LocalLibrary(library_id, li.path());
					lib_map[library_id] = lib;
					return lib;
				}
			}
		}

		return nullptr;
	}

	LibraryInfo get_library_info(qint8 id)
	{
		for(const LibraryInfo& li : all_libs)
		{
			if(li.id() == id){
				return li;
			}
		}

		return LibraryInfo();
	}

	void move_library(int row, int new_row)
	{
		all_libs.move(row, new_row);
	}
};


LibraryManager::LibraryManager() :
	SayonaraClass()
{
	_m = Pimpl::make<Private>();
	revert();
}

LibraryManager::~LibraryManager() {}

qint8 LibraryManager::add_library(const QString& name, const QString& path)
{
	if(path.isEmpty() || name.isEmpty()){
		return -1;
	}

	if(_m->contains_path(path)){
		return -1;
	}

	qint8 id = _m->get_next_id();
	LibraryInfo li(name, path, id);

	_m->all_libs << li;
	_m->lph->add_local_library(li, _m->all_libs.count() - 1);

	_settings->set(Set::Lib_AllLibraries, _m->all_libs);

	return id;
}

void LibraryManager::rename_library(qint8 id, const QString& new_name)
{
	_m->rename_library(id, new_name);
	_m->lph->rename_local_library(id, new_name);
	_settings->set(Set::Lib_AllLibraries, _m->all_libs);
}

void LibraryManager::remove_library(qint8 id)
{
	for(int i=0; i<_m->all_libs.size(); i++)
	{
		if(_m->all_libs[i].id() != id) {
			continue;
		}

		_m->lph->remove_local_library(id);

		LibraryInfo info = _m->all_libs.takeAt(i);
		LocalLibrary* library = _m->lib_map.take(info.id());
		if(library){
			delete library; library=nullptr;
		}

		_settings->set(Set::Lib_AllLibraries, _m->all_libs);
		break;
	}
}

void LibraryManager::move_library(int old_row, int new_row)
{
	_m->move_library(old_row, new_row);
	_m->lph->move_local_library(old_row, new_row);
	_settings->set(Set::Lib_AllLibraries, _m->all_libs);
}



QList<LibraryInfo> LibraryManager::get_all_libraries() const
{
	return _m->all_libs;
}


int LibraryManager::count() const
{
	return _m->all_libs.size();
}

LibraryInfo LibraryManager::get_library(qint8 id) const
{
	return _m->get_library_info(id);
}

LocalLibrary* LibraryManager::get_library_instance(qint8 id) const
{
	return _m->get_library(id);
}


void LibraryManager::set_library_path(qint8 library_id, const QString& library_path)
{
	_m->set_library_path(library_id, library_path);
}

qint8 LibraryManager::get_next_lib_id() const
{
	return _m->get_next_id();
}

void LibraryManager::revert()
{
	_m->all_libs = _settings->get(Set::Lib_AllLibraries);

	if(_m->all_libs.isEmpty()){
		QString old_path = _settings->get(Set::Lib_Path);
		if(!old_path.isEmpty()) {
			LibraryInfo li("Local Library", old_path, 0);
			_m->all_libs << li;
			_settings->set(Set::Lib_AllLibraries, _m->all_libs);
		}
	}
}

