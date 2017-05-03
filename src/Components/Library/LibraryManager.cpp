#include "LibraryManager.h"
#include "LibraryInfo.h"
#include "Helper/FileHelper.h"


struct LibraryManager::Private
{
	int cur_lib_idx;
	QList<LibraryInfo> all_libs;

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
};


LibraryManager::LibraryManager()
{
	_m = Pimpl::make<Private>();
	//_m->cur_lib_idx = _settings->get(Set::Lib_Current, _m->cur_lib);
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
	}

	//_settings->set(Set::Lib_Current, _m->cur_lib_idx);
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

