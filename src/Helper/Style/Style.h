/* Style.h */

/* Copyright (C) 2011-2016 Lucio Carreras
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


/*
 * Style.h
 *
 *  Created on: Oct 27, 2011
 *      Author: luke
 */

#ifndef STYLE_H_
#define STYLE_H_

#include "Helper/Helper.h"
#include <QPalette>

namespace Style{

	QString get_style(bool dark, QString font_family=QString(), int font_size=0);
}


#endif /* STYLE_H_ */
