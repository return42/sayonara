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
