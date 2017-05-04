#include "LibraryManager.h"
#include "Helper/Library/LibraryInfo.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/Settings.h"
#include "LocalLibrary.h"

#include <QMap>


struct LibraryManager::Private
{
	int cur_lib_idx;
	QList<LibraryInfo> all_libs;
	QMap<int, LocalLibrary*> lib_map;

	bool contains_path(const QString& path) const
	{
		return std::any_of(all_libs.begin(), all_libs.end(), [=](const LibraryInfo& li){
			return (li == path);
		});
	}

	bool has_current_library() const
	{
		return (cur_lib_idx >= 0 && cur_lib_idx <= all_libs.size());
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

	LocalLibrary* get_library(int idx)
	{
		if(idx >= 0 && idx < all_libs.size()){
			LocalLibrary* lib = new LocalLibrary(all_libs[idx].path());
			lib_map[idx] = lib;
		}

		if(!lib_map.contains(idx)){
			return nullptr;
		}

		return lib_map[idx];
	}
};


LibraryManager::LibraryManager() :
	SayonaraClass()
{
	_m = Pimpl::make<Private>();

	_m->cur_lib_idx = _settings->get(Set::Lib_CurIndex);
	_m->all_libs = _settings->get(Set::Lib_AllLibraries);

	if(_m->all_libs.isEmpty()){
		QString old_path = _settings->get(Set::Lib_Path);
		if(!old_path.isEmpty()) {
			LibraryInfo li("Local Library", old_path, 0);
			_m->all_libs << li;
			_m->cur_lib_idx = 0;

			_settings->set(Set::Lib_CurIndex, 0);
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

void LibraryManager::remove_library(int index)
{
	if(index < 0 || index >= _m->all_libs.size()) {
		return;
	}

	_m->all_libs.removeAt(index);

	if(index == _m->cur_lib_idx) {
		if(_m->all_libs.size() > 1) {
			_m->cur_lib_idx = 0;
		}

		else {
			_m->cur_lib_idx = -1;
		}

		_settings->set(Set::Lib_CurIndex, _m->cur_lib_idx);
	}

	_settings->set(Set::Lib_AllLibraries, _m->all_libs);
}

LibraryInfo LibraryManager::get_current_library() const
{
	if(_m->has_current_library()){
		return _m->all_libs[_m->cur_lib_idx];
	}

	return LibraryInfo();
}

QList<LibraryInfo> LibraryManager::get_all_libraries() const
{
	return _m->all_libs;
}

int LibraryManager::count() const
{
	return _m->all_libs.size();
}

LocalLibrary* LibraryManager::get_library_instance(int idx) const
{
	return _m->get_library(idx);
}

LocalLibrary* LibraryManager::get_current_library_instance() const
{
	return _m->get_library(_m->cur_lib_idx);
}

int LibraryManager::change_library(int idx)
{
	if(idx < 0 || idx >= count()){
		return _m->cur_lib_idx;
	}

	_m->cur_lib_idx = idx;
	_settings->set(Set::Lib_CurIndex, idx);
	return idx;
}

QString LibraryManager::get_current_library_path() const
{
	return get_current_library().path();
}



