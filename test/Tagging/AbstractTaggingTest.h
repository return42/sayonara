#ifndef ABSTRACTTAGGINGTEST_H
#define ABSTRACTTAGGINGTEST_H

#include <QObject>


class AbstractTaggingTest :
		public QObject
{
	Q_OBJECT

private:
	QString _resource_filename;
	QString _filename;

private:
	void init();
	void cleanup();
	void run();

protected:
	virtual void run_test(const QString& filename)=0;

protected slots:
	void id3_test();
	void xiph_test();
};

#endif // ABSTRACTTAGGINGTEST_H
