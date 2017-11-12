/* StreamRecorderUtils.h */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



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
