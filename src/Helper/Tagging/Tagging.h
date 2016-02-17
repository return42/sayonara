/* id3.h */

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


#ifndef TAGGING_H_
#define TAGGING_H_

#include "Helper/MetaData/MetaData.h"

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/fileref.h>

/**
 * @brief Tagging namespace
 * @ingroup Tagging
 */
namespace Tagging
{

	enum class Quality : quint8 {

		Fast=TagLib::AudioProperties::Fast,
		Standard=TagLib::AudioProperties::Average,
		Quality=TagLib::AudioProperties::Accurate,
		Dirty
	};

	bool getMetaDataOfFile(MetaData& md, Tagging::Quality quality=Tagging::Quality::Standard);
	bool setMetaDataOfFile(const MetaData& md);
}


#endif
