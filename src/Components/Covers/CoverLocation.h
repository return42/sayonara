/* CoverLocation.h */

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

#ifndef COVERLOCATION_H
#define COVERLOCATION_H

#include <QMetaType>
#include "Utils/Pimpl.h"

class QUrl;
class QString;
class QStringList;
class MetaData;
class Album;
class Artist;

namespace Cover
{
    /**
     * @brief The CoverLocation class
     * @ingroup Covers
     */
    class Location
    {
        PIMPL(Location)

    public:
        Location();
        ~Location();
        Location(const Location& cl);
        Location& operator=(const Location& cl);

        bool valid() const;
        QStringList local_paths() const;
        void add_local_path(const QString& path);
        QString local_path(int idx) const;
        QString cover_path() const;
        QString identifer() const;
        QStringList search_urls() const;
        void remove_first_search_url();

        bool has_search_urls() const;
        QString search_term() const;
        void set_search_term(const QString& search_term);
        QString to_string() const;


        /**
         * @brief creates CoverLocation by taking the md5 sum between album_name and artist_name
         * @param album_name Album name
         * @param artist_name Artist name
         * @return CoverLocation object
         */
        static Location get_cover_location(const QString& album_name, const QString& artist_name);

        /**
         * @brief overloaded. Picks major artist out of artists and calls
         *   get_cover_location(const QString& album_name, const QString& artist_name)
         * @param album_name Album name
         * @param artists List of artists
         * @return CoverLocation object
         */
        static Location get_cover_location(const QString& album_name, const QStringList& artists);


        /**
         * @brief overloaded. Calls
         *   get_cover_location(const QString& album_name, const QStringList& artists)
         * @param album
         * @return CoverLocation object
         */
        static Location get_cover_location(const Album& album);


        /**
         * @brief The slowest, but by far the most accurate function. Checks all local
         * cover paths by all known tracks for the album.
         * Fetches album from database and calls
         *  get_cover_location(const QString& album_name, const QStringList& artists)
         * @param album_id Album ID
         * @param db_id Database ID for album
         * @return CoverLocation object
         */



        /**
         * @brief Creates cover token of the form artist_<md5sum of artist>
         * @param artist Artist name
         * @return CoverLocation object
         */
        static Location get_cover_location(const QString& artist);


        /**
         * @brief overloaded. extracts artist name and calls
         *   get_cover_location(const QString& artist)
         * @param artist Artist object
         * @return CoverLocation object
         */
        static Location get_cover_location(const Artist& artist);


        /**
         * @brief overloaded.
         *   if MetaData::album_id < 0 calls
         *     get_cover_location(const QString& album_name, const QString& artist_name)
         *   else extract Album from database and calls
         *     get_cover_location(const Album& album)
         * @param Metadata object
         * @return  CoverLocation object
         */
        static Location get_cover_location(const MetaData& md);


        /**
         * @brief fetch a cover from a specific url
         * @param url url, the cover has to be fetched from
         * @param target_path path where the found image has to be saved
         * @return CoverLocation object
         */
        static Location get_cover_location(const QUrl& url, const QString& target_path);


        /**
         * @brief returns an invalid location
         * @return  CoverLocation object
         */
        static Location getInvalidLocation();

        /**
         * @brief returns if path is the same as the invalid location path
         * @param cover_path the path to be compared
         * @return
         */
        static bool isInvalidLocation(const QString& cover_path);


        /**
         * @brief returns the standard cover directory
         * @return usually ~/.Sayonara/covers
         */
        static QString get_cover_directory(const QString& append_path);

        QString preferred_path() const;
    };

}
Q_DECLARE_METATYPE(Cover::Location)
#endif // COVERLOCATION_H
