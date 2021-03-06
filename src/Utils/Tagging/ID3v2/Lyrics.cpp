/* Lyrics.cpp */

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



#include "Lyrics.h"
#include <taglib/tstring.h>

ID3v2::LyricsFrame::LyricsFrame(const TagLib::FileRef& f) :
	ID3v2Frame<QString, TagLib::ID3v2::UnsynchronizedLyricsFrame> (f, "USLT")
{}

ID3v2::LyricsFrame::~LyricsFrame() {}

void ID3v2::LyricsFrame::map_model_to_frame(const QString& model, TagLib::ID3v2::UnsynchronizedLyricsFrame* frame)
{
	QByteArray data = model.toUtf8();
	TagLib::ByteVector vec(data.constData(), data.size());
	TagLib::String str(vec);
	frame->setText(str);
}

void ID3v2::LyricsFrame::map_frame_to_model(const TagLib::ID3v2::UnsynchronizedLyricsFrame* frame, QString& model)
{
	TagLib::String tag_str = frame->text();

	QString str = QString::fromUtf8(frame->toString().toCString(false));
	model = str;
}

TagLib::ID3v2::Frame* ID3v2::LyricsFrame::create_id3v2_frame()
{
	return new TagLib::ID3v2::UnsynchronizedLyricsFrame(TagLib::String::UTF8);
}
