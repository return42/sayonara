#include <QTest>
#include "Helper/Tagging/Tagging.h"
#include "Helper/FileHelper.h"
#include "Helper/MetaData/MetaData.h"
#include "AbstractTaggingTest.h"

class LyricsTest : public AbstractTaggingTest
{
	Q_OBJECT

private:
	void run_test(const QString& filename) override;

private slots:
	void id3_test();
	void xiph_test();
};


void LyricsTest::run_test(const QString& filename)
{
	QString read_lyrics;
	QString lyrics = QString::fromUtf8("Those are söme lyrics фыва");
	MetaData md(filename);
	Tagging::getMetaDataOfFile(md);

	bool wrote_lyrics = Tagging::write_lyrics(md, lyrics);
	bool extracted_lyrics = Tagging::extract_lyrics(md, read_lyrics);

	QVERIFY(wrote_lyrics == true);
	QVERIFY(extracted_lyrics == true);
	QVERIFY(lyrics.compare(read_lyrics) == 0);
}

void LyricsTest::id3_test()
{
	AbstractTaggingTest::id3_test();
}

void LyricsTest::xiph_test()
{
	AbstractTaggingTest::xiph_test();
}

QTEST_MAIN(LyricsTest)

#include "LyricsTest.moc"

