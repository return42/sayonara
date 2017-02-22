#ifndef ALBUMCOVERFETCHTHREAD_H
#define ALBUMCOVERFETCHTHREAD_H

#include <QThread>
#include <QModelIndex>

#include "Helper/Pimpl.h"

class CoverLocation;
class Album;
class AlbumCoverFetchThread : public QThread
{
    Q_OBJECT
    PIMPL(AlbumCoverFetchThread)

signals:
    void sig_next(const QString& hash, const CoverLocation& cl);


protected:
    void run() override;

public:
    AlbumCoverFetchThread(QObject* parent=nullptr);
    ~AlbumCoverFetchThread();

    void add_data(const QString& hash, const CoverLocation& cl);
    void done(bool success);
};



#endif // ALBUMCOVERFETCHTHREAD_H
