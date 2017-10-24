#include "Utils/MetaData/MetaData.h"
#include "Utils/StreamRecorder/StreamRecorderUtils.h"

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
    void invalid_chars_test();
};


void StreamRecorderUtilsTest::tag_mismatch_test()
{
    int invalid_idx;
    QString templ  = "<ar> - <t> - <<m>/bla.mp3";
    Utils::ErrorCode e = Utils::validate_template(templ, &invalid_idx);

    qDebug() << __FUNCTION__ << (int) e;

    QVERIFY(e == Utils::ErrorCode::BracketError);
}

void StreamRecorderUtilsTest::filename_test()
{
    MetaData md;
    md.title = "Bli";
    md.set_artist("bla");

    QString sr_path("/path/to/nowhere");

    QDate d = QDateTime::currentDateTime().date();
    QTime t = QDateTime::currentDateTime().time();

    QString templ = "<t> <m>_<y>-/<ds>+df3-<min>_<t>-<d>-/<dl>df<ar>";

    int idx;
    Utils::ErrorCode err = Utils::validate_template(templ, &idx);
    QVERIFY(err == Utils::ErrorCode::OK);

    Utils::TargetPaths path = Utils::full_target_path(sr_path, templ, md);

    QString other_path = sr_path + "/" + QString("%1 %2_%3-/%4+df3-%5_%6-%7-/%8df%9.mp3")
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
    int invalid_idx;
    QString templ = "<t> <m>_<y>-/<fs>+df3-<min>_<t>-<d>-/<dl>df<a>";
    Utils::ErrorCode e = Utils::validate_template(templ, &invalid_idx);

    qDebug() << __FUNCTION__ << (int) e;

    QVERIFY(e == Utils::ErrorCode::UnknownTag);
}

void StreamRecorderUtilsTest::invalid_chars_test()
{
    int invalid_idx;
    QString templ = "<t> <m>_<y>-/<ds>:df3-<min>_<t>-<d>-/<dl>df<ar>";
    Utils::ErrorCode e = Utils::validate_template(templ, &invalid_idx);

    qDebug() << __FUNCTION__ << (int) e;

    QVERIFY(e == Utils::ErrorCode::InvalidChars);
}



QTEST_MAIN(StreamRecorderUtilsTest)

#include "StreamRecorderUtilsTest.moc"
