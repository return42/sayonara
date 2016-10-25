#ifndef DATABASESEARCHMODE_H
#define DATABASESEARCHMODE_H

#include "Helper/Pimpl.h"
#include "Helper/Library/SearchMode.h"
#include <QSqlDatabase>

class DatabaseSearchMode
{
    PIMPL(DatabaseSearchMode)

private:
    void init();

protected:
    DatabaseSearchMode();
    DatabaseSearchMode(const QSqlDatabase& db);

public:

    virtual ~DatabaseSearchMode();

    Library::SearchModeMask search_mode();
    void update_search_mode();
};

#endif // DATABASESEARCHMODE_H
