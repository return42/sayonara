#ifndef LYRICS_FRAME_H
#define LYRICS_FRAME_H

#include <QString>
#include "ID3v2Frame.h"
#include <taglib/unsynchronizedlyricsframe.h>

namespace ID3v2
{
	class LyricsFrame :
			public ID3v2Frame<QString, TagLib::ID3v2::UnsynchronizedLyricsFrame>
	{
		public:
			explicit LyricsFrame(const TagLib::FileRef& f);
			~LyricsFrame();

			void map_model_to_frame(const QString& model, TagLib::ID3v2::UnsynchronizedLyricsFrame* frame) override;
			void map_frame_to_model(const TagLib::ID3v2::UnsynchronizedLyricsFrame* frame, QString& model) override;

			TagLib::ID3v2::Frame* create_id3v2_frame() override;
	};
}

#endif // LYRICS_FRAME_H
