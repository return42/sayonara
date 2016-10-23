#include "Database/DatabaseSearchMode.h"
#include "Database/SayonaraQuery.h"


struct DatabaseSearchMode::Private
{
    QSqlDatabase db;
    bool initialized;
    Library::SearchModeMask search_mode;


    Private()
    {
	initialized = false;
	search_mode = Library::CaseInsensitve;
    }
};

DatabaseSearchMode::DatabaseSearchMode()
{
    _m = Pimpl::make<DatabaseSearchMode::Private>();
}

DatabaseSearchMode::DatabaseSearchMode(const QSqlDatabase& db) :
    DatabaseSearchMode()
{
    _m->db = db;
}

DatabaseSearchMode::~DatabaseSearchMode(){}


void DatabaseSearchMode::init()
{
    if(_m->initialized){
	return;
    }

    SayonaraQuery q (_m->db);
    q.prepare("SELECT value FROM settings WHERE key = 'lib_search_mode';");

    if(q.exec()) {
	if (q.next()) {
	    _m->initialized = true;
	    _m->search_mode = q.value(0).toInt();
	}
    }
}

Library::SearchModeMask DatabaseSearchMode::search_mode()
{
   init();

   return _m->search_mode;
}

void DatabaseSearchMode::update_search_mode()
{
    _m->initialized = false;
    init();
}
