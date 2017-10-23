#ifndef STREAMRECORDERUTILS_H
#define STREAMRECORDERUTILS_H

#include <QPair>
#include <QList>
#include <QString>
#include <QHash>
#include <QDateTime>
#include <QList>

class MetaData;

namespace StreamRecorder
{
    namespace Utils
    {
        enum ErrorCode
        {
            OK=1,
            BracketError,
            UnknownTag,
            MissingUniqueTag,
            InvalidChars
        };

        // filename, playlistname
        using TargetPaths=QPair<QString, QString>;

        QList<QString> supported_tags();
        QList<QPair<QString, QString>> descriptions();

        ErrorCode validate_template(const QString& target_path_template, int* invalid_idx);

        QString target_path_template_default(bool use_session_path);

        TargetPaths full_target_path(const QString& sr_path, const QString& path_template, const MetaData& md);

        QString parse_error_code(ErrorCode err);
    }
}

#endif // STREAMRECORDERUTILS_H
