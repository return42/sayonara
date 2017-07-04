/* typedefs.h */

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

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

//#include <QList>
//#include <QString>
//#include <QPair>
//#include <QtGlobal>

#include <cstdint>

class QString;
template <typename A, typename B> struct QPair;
template <typename T> class QList;

/**
 * @brief Sayonara Typedefs
 * @ingroup Helper
 */
typedef QPair<QString, QString> StringPair;
typedef QList<int> IntList;
typedef QList<int> IDList;
typedef QList<int> IdxList;
typedef QList<bool> BoolList;
typedef int32_t ArtistID;
typedef int32_t AlbumID;
typedef int32_t TrackID;
typedef int32_t ID;


#endif
