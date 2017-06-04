#include <QTest>
#include "AbstractTaggingTest.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaData.h"


class DiscnumberTest : public AbstractTaggingTest
{
	Q_OBJECT

private:
	void run_test(const QString& filename) override;

private slots:
	void id3_test();
	void xiph_test();
};


void DiscnumberTest::run_test(const QString& filename)
{
	QString album_artist = QString::fromUtf8("Motörhead фыва");
	MetaData md(filename);
	MetaData md2(filename);

	Tagging::getMetaDataOfFile(md);
	QVERIFY(md.discnumber == 5);

	md.discnumber = 8;
	md.n_discs = 9;
	Tagging::setMetaDataOfFile(md);

	Tagging::getMetaDataOfFile(md2);
	QVERIFY(md2.discnumber == 8);
	QVERIFY(md2.n_discs == 9);

	md.discnumber = 10;
	md.n_discs = 12;
	Tagging::setMetaDataOfFile(md);

	Tagging::getMetaDataOfFile(md2);
	QVERIFY(md2.discnumber == 10);
	QVERIFY(md2.n_discs == 12);
}

void DiscnumberTest::id3_test()
{
	AbstractTaggingTest::id3_test();
}

void DiscnumberTest::xiph_test()
{
	AbstractTaggingTest::xiph_test();
}

QTEST_MAIN(DiscnumberTest)

#include "DiscnumberTest.moc"
