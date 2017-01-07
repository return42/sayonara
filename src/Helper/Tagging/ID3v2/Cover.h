/* Cover.h */

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

#ifndef COVER_H
#define COVER_H

#include "AbstractFrame.h"
#include "Tagging/Models/Cover.h"

#include <QByteArray>
#include <QString>
#include <taglib/attachedpictureframe.h>

namespace ID3v2Frame
{
	/**
	 * @brief The DiscnumberFrame class
	 * @ingroup Tagging
	 */
	class CoverFrame :
			public AbstractFrame<Models::Cover, TagLib::ID3v2::AttachedPictureFrame>
	{
		public:
			CoverFrame(const TagLib::FileRef& f);
			virtual ~CoverFrame();

			void map_model_to_frame() override;
			void map_frame_to_model() override;

			TagLib::ID3v2::Frame* create_id3v2_frame() override;
	};
}

#endif // COVER_H
