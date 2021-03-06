/* EqualizerHandler.h */

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



#ifndef EQUALIZERHANDLER_H
#define EQUALIZERHANDLER_H

#include "Utils/Pimpl.h"

struct _GstElement;
typedef struct _GstElement GstElement;

class QString;

namespace Pipeline
{
    class EqualizerHandler
    {
        PIMPL(EqualizerHandler)

    public:
        EqualizerHandler();
        virtual ~EqualizerHandler();

        void init_equalizer();
        void set_band(int, int val);

        virtual GstElement* get_equalizer_element() const=0;
    };
}

#endif // EQUALIZERHANDLER_H
