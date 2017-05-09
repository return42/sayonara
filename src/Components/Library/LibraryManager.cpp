#include "LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "LocalLibrary.h"

#include <QMap>

struct LibraryManager::Private
{
    QList<LibraryInfo> all_libs;
    QMap<int, LocalLibrary*> lib_map;

    bool contains_path(const QString& path) const
    {
	return std::any_of(all_libs.begin(), all_libs.end(), [=](const LibraryInfo& li){
	    return (li == path);
	});
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
	int id=0;
	QList<int> ids;
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

    LibraryInfo get_library_info(int id)
    {
	for(const LibraryInfo& li : all_libs)
	{
	    if(li.id() == id){
		return li;
	    }
	}

	return LibraryInfo();
    }
};


LibraryManager::LibraryManager() :
    SayonaraClass()
{
    _m = Pimpl::make<Private>();
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

LibraryManager::~LibraryManager() {}

bool LibraryManager::add_library(const QString& name, const QString& path)
{
    if(path.isEmpty() || name.isEmpty()){
	return false;
    }

    if(_m->contains_path(path)){
	return false;
    }

    int id = _m->get_next_id();
    LibraryInfo li(
                name,
                path,
                id
                );

    _m->all_libs << li;
    _settings->set(Set::Lib_AllLibraries, _m->all_libs);
    return true;
}

void LibraryManager::remove_library(int id)
{
    for(int i=0; i<_m->all_libs.size(); i++){
	if(_m->all_libs[i].id() == id){
	    _m->all_libs.removeAt(i);
	    break;
	}
    }

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

LibraryInfo LibraryManager::get_library(int id) const
{
    return _m->get_library_info(id);
}

LocalLibrary* LibraryManager::get_library_instance(int id) const
{
    return _m->get_library(id);
}


void LibraryManager::set_library_path(qint8 library_id, const QString& library_path)
{
    _m->set_library_path(library_id, library_path);
}
