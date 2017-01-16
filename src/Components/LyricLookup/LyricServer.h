/* LyricServer.h */

/* Copyright (C) 2012  Lucio Carreras
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

#ifndef LYRICSERVER_H_
#define LYRICSERVER_H_

#include <QString>
#include <QMap>


/**
 * @brief The ServerTemplate struct
 * @ingroup Lyrics
 */
struct ServerTemplate
{
    QString display_str;
    QString server_address;
    QMap<QString, QString> replacements;
    QString call_policy;
    QMap<QString, QString> start_end_tag;
    bool include_start_tag;
    bool include_end_tag;
    bool is_numeric;
    bool to_lower;
    QString error;

    void addReplacement(const QString& rep, const QString& rep_with);
    void print_xml() const;
    void print_json() const;
};

#endif /* LYRICSERVER_H_ */
