#ifndef GENREFETCHER_H
#define GENREFETCHER_H

#include <QObject>
#include "Helper/Pimpl.h"

class QStringList;
class MetaDataList;
class LocalLibrary;

class GenreFetcher :
        public QObject
{
    Q_OBJECT
    PIMPL(GenreFetcher)

signals:
    void sig_genres_fetched();
    void sig_progress(int progress);
    void sig_finished();


public:
    explicit GenreFetcher(QObject* parent=nullptr);
    ~GenreFetcher();


    QStringList genres() const;

    void add_genre_to_md(const MetaDataList& v_md, const QString& genre);

    void create_genre(const QString& genre);
    void delete_genre(const QString& genre);
    void rename_genre(const QString& old_name, const QString& new_name);

    void set_local_library(LocalLibrary* local_library);

private slots:
    void reload_genres();
    void metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new);
    void metadata_deleted(const MetaDataList& v_md_deleted);

    void tag_edit_finished();
};


#endif // GENREFETCHER_H
