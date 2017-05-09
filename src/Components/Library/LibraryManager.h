#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "Helper/Singleton.h"
#include "Helper/Pimpl.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QList>

class LibraryInfo;
class LocalLibrary;
class LocalLibraryContainer;

class LibraryManager :
		public SayonaraClass
{
    PIMPL(LibraryManager)
    SINGLETON(LibraryManager)

public:
    bool add_library(const QString& name, const QString& path);
    void remove_library(int id);

    QList<LibraryInfo> get_all_libraries() const;
    LibraryInfo get_library(int id) const;
    int count() const;

    LocalLibrary* get_library_instance(int id) const;
    void set_library_path(qint8 library_id, const QString& library_path);
};


#endif // LIBRARYMANAGER_H
