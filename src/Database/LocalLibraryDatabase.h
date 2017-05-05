#ifndef LOCALLIBRARYDATABASE_H
#define LOCALLIBRARYDATABASE_H

#include "LibraryDatabase.h"

class LocalLibraryDatabase :
		public LibraryDatabase
{
public:
	LocalLibraryDatabase(qint8 library_id);
	~LocalLibraryDatabase();
};

#endif // LOCALLIBRARYDATABASE_H
