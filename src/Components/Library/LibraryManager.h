#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QList>

class LibraryInfo;

class LibraryManager :
		public SayonaraClass
{
	PIMPL(LibraryManager)
	SINGLETON(LibraryManager)

public:
	bool add_library(const QString& name, const QString& path);
	void remove_library(int index);

	LibraryInfo get_current_library() const;
	QList<LibraryInfo> get_all_libraries() const;
	int count() const;

};


#endif // LIBRARYMANAGER_H
