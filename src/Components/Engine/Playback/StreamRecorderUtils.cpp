#include "StreamRecorderUtils.h"
#include "Utils/MetaData/MetaData.h"
#include "Utils/FileUtils.h"
#include "Utils/Language.h"

#include <QString>
#include <QDateTime>

using namespace StreamRecorder;

Utils::TargetPaths Utils::full_target_path(const QString& sr_path, const QString &path_template, const MetaData &md)
{
    if(!validate_template(path_template)){
        return Utils::TargetPaths();
    }

    Utils::TargetPaths ret;
    QString target_path = path_template;

    QTime time = QDateTime::currentDateTime().time();
    QDate date = QDateTime::currentDateTime().date();

    target_path.replace("<hr>",		QString::number(time.hour()));
    target_path.replace("<min>",	QString::number(time.minute()));
    target_path.replace("<dd>",		QString::number(date.day()));
    target_path.replace("<sd>",		QDate::shortDayName(date.dayOfWeek()));
    target_path.replace("<ld>",		QDate::longDayName(date.dayOfWeek()));
    target_path.replace("<m>",		QString::number(date.month()));
    target_path.replace("<sm>",		QDate::shortMonthName(date.month()));
    target_path.replace("<lm>",		QDate::longMonthName(date.month()));
    target_path.replace("<yy>",		QString::number(date.year()));
    target_path.replace("<tn>",		QString::number(md.track_num));
    target_path.replace("<t>",      md.title.trimmed());
    target_path.replace("<ar>",     md.artist().trimmed());
    target_path.replace("<rs>",     md.album().trimmed());

    if(!target_path.endsWith(".mp3")){
        target_path += ".mp3";
    }

    ret.first = Util::File::clean_filename(sr_path + "/" + target_path);
    ret.second = Util::File::clean_filename(Util::File::get_parent_directory(ret.first) + "/playlist.m3u");

    return ret;
}


Utils::TargetPaths Utils::full_target_path(const QString &path_template, const MetaData &md)
{
   return full_target_path(QString(), path_template, md);
}

QString Utils::full_target_path_template(const QString& sr_path, const QString& target_path_template)
{
    QString ret(sr_path);

    ret += "/" + target_path_template;

    return Util::File::clean_filename(ret);
}

QString Utils::full_target_path_template_default(const QString& sr_path, bool use_session_path)
{
    QString ret(sr_path);

    if(use_session_path)
    {
        ret += "/<sd>_<yy>-<sm>-<ld>_<hh>h<min>/";
    }

    ret += "<tn> - <ar> - <title>.mp3";

    return Util::File::clean_filename(ret);
}

Utils::ErrorCode Utils::validate_template(const QString& target_path_template)
{
    int is_open = 0;
    int is_close = 0;
    for(QChar c : target_path_template)
    {
        if(c == '<')
        {
            is_open++;
            if(std::abs(is_close - is_open) > 1) {
                return Utils::BracketError;
            }
        }

        if(c == '>')
        {
            is_close++;
            if(std::abs(is_close - is_open) > 1) {
                return Utils::BracketError;
            }
        }
    }

    if(is_open != is_close){
        return Utils::BracketError;
    }


    QList<QString> tags = supported_tags();

    QRegExp re(".*<(.*)>.*");
    re.setMinimal(true);

    int idx = re.indexIn(target_path_template);
    while(idx >= 0 && idx < target_path_template.size())
    {
        QString tag = re.cap(0);
        if(!tags.contains(tag)){
            return Utils::UnknownTag;
        }

        idx = re.indexIn(target_path_template, idx + 1);
    }


    return Utils::OK;
}

QList<QString> Utils::supported_tags()
{
    QList<QString> ret;

    ret << "hr";
    ret << "min";
    ret << "dd";
    ret << "sd";
    ret << "ld";
    ret << "m";
    ret << "sm";
    ret << "lm";
    ret << "yy";
    ret << "tn";
    ret << "t";
    ret << "ar";
    ret << "rs";

    return ret;
}


