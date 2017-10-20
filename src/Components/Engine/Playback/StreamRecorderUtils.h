#ifndef STREAMRECORDERUTILS_H
#define STREAMRECORDERUTILS_H

#include <QPair>
#include <QList>
#include <QString>
#include <QHash>
#include <QDateTime>

#include "Utils/Language.h"

class MetaData;

namespace StreamRecorder
{
    namespace Utils
    {
        enum ErrorCode
        {
            OK,
            BracketError,
            UnknownTag,
        };

        // filename, playlistname
        using TargetPaths=QPair<QString, QString>;

        QString full_target_path_template_default(const QString& sr_path, bool use_session_path);
        QString full_target_path_template(const QString& sr_path, const QString& target_path_template);
        TargetPaths full_target_path(const QString& path_template, const MetaData& md);
        TargetPaths full_target_path(const QString& sr_path, const QString& path_template, const MetaData& md);

        ErrorCode validate_template(const QString& target_path_template);

        QList<QString> supported_tags();
        inline QList<QPair<QString, QString>> descriptions()
        {
            QList<QPair<QString, QString>> ret;

            QDate d = QDateTime::currentDateTime().date();

            ret << QPair<QString, QString>("min", Lang::get(Lang::Minutes));
            ret << QPair<QString, QString>("hr", Lang::get(Lang::Hours));
            ret << QPair<QString, QString>("dd", Lang::get(Lang::Days) + " (" + QString::number(d.day()) + ")");
            ret << QPair<QString, QString>("sd", Lang::get(Lang::Days) + " (" + QDate::shortDayName(d.dayOfWeek()) + ")");
            ret << QPair<QString, QString>("ld", Lang::get(Lang::Days) + " (" + QDate::longDayName(d.dayOfWeek()) + ")");
            ret << QPair<QString, QString>("m", Lang::get(Lang::Months) + " (" + QString::number(d.month()) + ")");
            ret << QPair<QString, QString>("sm", Lang::get(Lang::Months) + " (" + QDate::shortMonthName(d.month()) + ")");
            ret << QPair<QString, QString>("lm", Lang::get(Lang::Months) + " (" + QDate::longMonthName(d.month()) + ")");
            ret << QPair<QString, QString>("yy", Lang::get(Lang::Year));
            ret << QPair<QString, QString>("tn", Lang::get(Lang::TrackNo));
            ret << QPair<QString, QString>("t", Lang::get(Lang::Title));
            ret << QPair<QString, QString>("ar", Lang::get(Lang::Artist));
            ret << QPair<QString, QString>("rs", "Radio station");

            return ret;
        }
    }
}

#endif // STREAMRECORDERUTILS_H
