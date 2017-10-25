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
        enum class ErrorCode : uint8_t
        {
            OK=1,
            BracketError,
            UnknownTag,
            MissingUniqueTag,
            InvalidChars,
            Empty
        };

        // filename, playlistname
        using TargetPaths=QPair<QString, QString>;

        QList<QString> supported_tags();
        QList<QPair<QString, QString>> descriptions();

        ErrorCode validate_template(const QString& target_path_template, int* invalid_idx);

        QString target_path_template_default(bool use_session_path);

        TargetPaths full_target_path(const QString& sr_path, const QString& path_template, const MetaData& md, const QDate& d, const QTime& t);

        QString parse_error_code(ErrorCode err);
    }
}

#endif // STREAMRECORDERUTILS_H
