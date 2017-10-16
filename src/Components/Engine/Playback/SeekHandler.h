/* SeekHandler.h */

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



#ifndef SEEKHANDLER_H
#define SEEKHANDLER_H

#include "Utils/Pimpl.h"
#include "Components/Engine/gstfwd.h"

namespace Pipeline
{
    class SeekHandler
    {
        PIMPL(SeekHandler)

    public:
        SeekHandler();
        virtual ~SeekHandler();

        int64_t seek_rel(double percent, int64_t ref_ns);
        int64_t seek_abs(int64_t ns);
        int64_t seek_nearest(int64_t ns);

        virtual GstElement* get_source() const=0;
    };
}

#endif // SEEKHANDLER_H
