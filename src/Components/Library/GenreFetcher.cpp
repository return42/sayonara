#include "GenreFetcher.h"

#include "Components/Library/LocalLibrary.h"
#include "Components/TagEdit/MetaDataChangeNotifier.h"
#include "Components/TagEdit/TagEdit.h"
#include "Database/DatabaseConnector.h"
#include "Helper/MetaData/MetaDataList.h"

struct GenreFetcher::Private
{
    LocalLibrary* local_library=nullptr;
    QStringList genres;
    QStringList additional_genres; // empty genres that are inserted
    TagEdit* tag_edit=nullptr;

    Private() {}

    LibraryDatabase* get_local_library_db()
    {
	if(!local_library){
	    return nullptr;
	}

	qint8 library_id = local_library->library_id();

	DatabaseConnector* db = DatabaseConnector::getInstance();
	LibraryDatabase* lib_db = db->library_db(library_id, 0);
	return lib_db;
    }
};

GenreFetcher::GenreFetcher(QObject* parent) :
    QObject(parent)
{
    _m = Pimpl::make<Private>();
    _m->tag_edit = new TagEdit(this);

    MetaDataChangeNotifier* mcn = MetaDataChangeNotifier::getInstance();

    connect(mcn, &MetaDataChangeNotifier::sig_metadata_changed, this, &GenreFetcher::metadata_changed);
    connect(mcn, &MetaDataChangeNotifier::sig_metadata_deleted, this, &GenreFetcher::metadata_deleted);
    connect(_m->tag_edit, &TagEdit::sig_progress, this, &GenreFetcher::sig_progress);
    connect(_m->tag_edit, &TagEdit::finished, this, &GenreFetcher::tag_edit_finished);
}

GenreFetcher::~GenreFetcher() {}

void GenreFetcher::reload_genres()
{
    LibraryDatabase* db = _m->get_local_library_db();
    if(!db){
	return;
    }

    _m->genres = db->getAllGenres();

    emit sig_genres_fetched();
}

QStringList GenreFetcher::genres() const
{
    QStringList genres(_m->genres);
    genres << _m->additional_genres;

    return genres;
}

void GenreFetcher::metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)
{
    Q_UNUSED(v_md_old)
    Q_UNUSED(v_md_new)

    reload_genres();
}

void GenreFetcher::metadata_deleted(const MetaDataList& v_md_deleted)
{
    Q_UNUSED(v_md_deleted)

    reload_genres();
}

void GenreFetcher::tag_edit_finished()
{
    emit sig_finished();

    reload_genres();
}

void GenreFetcher::add_genre_to_md(const MetaDataList& v_md, const QString& genre)
{
    _m->tag_edit->set_metadata(v_md);

    for(int i=0; i<v_md.size(); i++){
	_m->tag_edit->add_genre(i, genre);
    }

    _m->tag_edit->commit();
    emit sig_progress(0);
}

void GenreFetcher::create_genre(const QString& genre)
{
    _m->additional_genres << genre;
    emit sig_genres_fetched();
}

void GenreFetcher::delete_genre(const QString& genre)
{
    if(_m->local_library){
	_m->local_library->delete_genre(genre);
    }
}

void GenreFetcher::rename_genre(const QString& old_name, const QString& new_name)
{
    if(_m->local_library){
	_m->local_library->rename_genre(old_name, new_name);
    }
}

void GenreFetcher::set_local_library(LocalLibrary* local_library)
{
    _m->local_library = local_library;
    connect(_m->local_library, &LocalLibrary::sig_reloading_library_finished,
            this, &GenreFetcher::reload_genres);

    reload_genres();
}



