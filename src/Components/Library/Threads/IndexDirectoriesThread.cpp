#include "IndexDirectoriesThread.h"
#include "Helper/Set.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaDataList.h"

struct IndexDirectoriesThread::Private
{
	MetaDataList	v_md;
	QStringList		directories;
};

IndexDirectoriesThread::IndexDirectoriesThread(const MetaDataList& v_md)
{
	_m = new IndexDirectoriesThread::Private();
	_m->v_md = v_md;
}

IndexDirectoriesThread::~IndexDirectoriesThread()
{
	delete _m; _m = nullptr;
}


QStringList IndexDirectoriesThread::get_directories() const
{
	return _m->directories;
}

void IndexDirectoriesThread::run()
{
	_m->directories.clear();

	SP::Set<QString> paths;
	for(const MetaData& md : _m->v_md)
	{
		paths.insert(Helper::File::get_parent_directory(md.filepath()));
	}

	for(auto it=paths.begin(); it!=paths.end(); it++){
		_m->directories << *it;
	}
}

