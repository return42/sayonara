#include "Components/Engine/Playback/StreamRecorderUtils.h"
#include "Utils/MetaData/MetaData.h"

#include <QObject>
#include <QTest>
#include <QDateTime>
#include <QDebug>

using namespace StreamRecorder;

class StreamRecorderUtilsTest : public QObject
{
    Q_OBJECT
    private slots:

    void tag_mismatch_test();
    void filename_test();
    void invalid_tag_test();
};


void StreamRecorderUtilsTest::tag_mismatch_test()
{
   QString templ  = "<ar> - <t> - <<m>/bla.mp3";
   Utils::ErrorCode e = Utils::validate_template(templ);

   qDebug() << __FUNCTION__ << e;

   QVERIFY(e == Utils::BracketError);
}

void StreamRecorderUtilsTest::filename_test()
{
    MetaData md;
    md.title = "Bli";
    md.set_artist("bla");

    QString sr_path("/path/to/nowhere");

    QDate d = QDateTime::currentDateTime().date();
    QTime t = QDateTime::currentDateTime().time();

    QString templ = "<t> <m>_<yy>-/<sd>:df3-<min>_<t>-<dd>-/<ld>df<ar>";
    Utils::TargetPaths path = Utils::full_target_path(sr_path, templ, md);

    QString other_path = sr_path + "/" + QString("%1 %2_%3-/%4:df3-%5_%6-%7-/%8df%9.mp3")
                          .arg("Bli")
                          .arg(d.month())
                          .arg(d.year())
                          .arg(QDate::shortDayName(d.dayOfWeek()))
                          .arg(t.minute())
                          .arg("Bli")
                          .arg(d.day())
                          .arg(QDate::longDayName(d.dayOfWeek()))
                          .arg("bla");

    qDebug() << path;
    qDebug() << other_path;

    QVERIFY(path.first.compare(other_path) == 0);
}

void StreamRecorderUtilsTest::invalid_tag_test()
{
    QString templ = "<t> <m>_<yy>-/<sf>:df3-<min>_<t>-<dd>-/<ld>df<a>";
    Utils::ErrorCode e = Utils::validate_template(templ);

    qDebug() << __FUNCTION__ << e;

    QVERIFY(e == Utils::UnknownTag);
}


QTEST_MAIN(StreamRecorderUtilsTest)

#include "StreamRecorderUtilsTest.moc"
