/* Popularimeter.cpp */

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

#include "Popularimeter.h"

ID3v2Frame::PopularimeterFrame::PopularimeterFrame(const TagLib::FileRef& f) :
	AbstractFrame<Models::Popularimeter, TagLib::ID3v2::PopularimeterFrame>(f, "POPM") {}

ID3v2Frame::PopularimeterFrame::~PopularimeterFrame() {}

TagLib::ID3v2::Frame* ID3v2Frame::PopularimeterFrame::create_id3v2_frame()
{
	return new TagLib::ID3v2::PopularimeterFrame();
}

void ID3v2Frame::PopularimeterFrame::map_model_to_frame()
{
	_frame->setEmail(TagLib::String(_data_model.email.toUtf8().data(), TagLib::String::UTF8));
	_frame->setCounter(_data_model.playcount);
	_frame->setRating(_data_model.rating_byte);
	_frame->render();
}

void ID3v2Frame::PopularimeterFrame::map_frame_to_model()
{
	_data_model.playcount = _frame->counter();

	_data_model.email = QString::fromLatin1(_frame->email().toCString());
	_data_model.rating_byte = (quint8) _frame->rating();
}
