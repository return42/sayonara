#include "LocalLibraryDatabase.h"

LocalLibraryDatabase::LocalLibraryDatabase(qint8 library_id) :
	LibraryDatabase("player.db", 0, library_id)
{

}

LocalLibraryDatabase::~LocalLibraryDatabase()
{

}

