#ifndef ABSTRACTCOVERLOOKUP_H
#define ABSTRACTCOVERLOOKUP_H

#include <QObject>

/**
 * @brief The CoverLookupInterface class
 * @ingroup Covers
 */
class AbstractCoverLookup :
		public QObject
{
	Q_OBJECT

signals:
	void sig_cover_found(const QString& file_path);
	void sig_finished(bool success);

public slots:
	virtual void stop()=0;

public:
	explicit AbstractCoverLookup(QObject* parent=nullptr);
	virtual ~AbstractCoverLookup();
};



#endif // ABSTRACTCOVERLOOKUP_H
