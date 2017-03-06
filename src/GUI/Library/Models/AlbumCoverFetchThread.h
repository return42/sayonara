#ifndef ALBUMCOVERFETCHTHREAD_H
#define ALBUMCOVERFETCHTHREAD_H

#include <QThread>
#include <QModelIndex>

#include "Helper/Pimpl.h"

class CoverLocation;
class Album;
/**
 * @brief This class organizes requests for new Covers for the AlbumCoverView.
 * When looking for covers, not all requests should be fired simultaneously,
 * so there should be a buffer assuring that covers are found one by one.
 * Albums are organized by hashes, each album has a CoverLocation. A new
 * request is added to the queue by calling add_data(). A new request is
 * handled BEFORE old requests. The thread is locked until the done() function
 * is called. The thread emits the signal sig_next(). The cover location
 * and the hash which should be processed next can be fetched by current_hash()
 * and current_cover_location().
 */
class AlbumCoverFetchThread : public QThread
{
    Q_OBJECT
    PIMPL(AlbumCoverFetchThread)

signals:
	void sig_next();

protected:
    void run() override;

public:
    AlbumCoverFetchThread(QObject* parent=nullptr);
    ~AlbumCoverFetchThread();

	/**
	 * @brief add_data Add a new album request
	 * @param hash hashed album info
	 * @param cl Cover Location of the album
	 */
    void add_data(const QString& hash, const CoverLocation& cl);

	/**
	 * @brief done Should be called when processing of the next
	 * cover should take place
	 * @param success not evalutated
	 */
    void done(bool success);

	/**
	 * @brief stop Stop the thread
	 */
	void stop();

	/**
	 * @brief Get the current processed hash valud
	 * @return
	 */
	QString current_hash() const;

	/**
	 * @brief Get the curren processed cover location
	 * @return
	 */
	CoverLocation current_cover_location() const;
};

#endif // ALBUMCOVERFETCHTHREAD_H
