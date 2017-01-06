/* singleton.h */

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



#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON_QOBJECT(class_name) protected: \
						explicit class_name (QObject* object=0); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();


#define SINGLETON(class_name) protected: \
						class_name (); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();

#endif
