#include "SymlinkUtils.h"

#include "Components/Library/LibraryManager.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"

#include "Utils/Library/LibraryInfo.h"
#include "Utils/typedefs.h"

#include <QPair>
#include <QString>

#include <algorithm>

QString SymlinkUtils::filepath_by_sympath(const QString &sympath)
{
	using StringPair = QPair<QString, QString>;
	QList<StringPair> info_sympaths;

	DB::Connector* db = DB::Connector::instance();
	DB::LibraryDatabases library_dbs = db->library_dbs();

	for(DB::LibraryDatabase* lib_db : library_dbs)
	{
		LibraryId lib_id = lib_db->library_id();
		if(lib_id < 0){
			continue;
		}

		Library::Info info = Library::Manager::instance()->library_info(lib_id);
		info_sympaths << StringPair(info.symlink_path(), info.path());
	}

	std::sort(info_sympaths.begin(), info_sympaths.end(), [](const StringPair& sp1, const StringPair& sp2){
		return (sp1.first.size() > sp2.first.size());
	});

	for(const StringPair& sp : info_sympaths)
	{
		if(sympath.contains(sp.first))
		{
			QString full_path(sympath);
			full_path.replace(sp.first, sp.second);
			return full_path;
		}
	}

	return QString();
}
