#ifndef INDEXDIRECTORIESTHREAD_H
#define INDEXDIRECTORIESTHREAD_H

#include <QStringList>
#include <QThread>

class MetaDataList;
class IndexDirectoriesThread :
		public QThread
{
public:
	IndexDirectoriesThread(const MetaDataList& v_md);
	virtual ~IndexDirectoriesThread();

	QStringList get_directories() const;

protected:
	void run() override;

private:
	struct Private;
	IndexDirectoriesThread::Private* _m=nullptr;
};

#endif // INDEXDIRECTORIESTHREAD_H
