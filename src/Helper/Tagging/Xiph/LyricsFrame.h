#ifndef LYRICS_H
#define LYRICS_H

#include "XiphFrame.h"

namespace Xiph
{
	class LyricsFrame :
            public Xiph::XiphFrame<QString>
    {
        public:
			explicit LyricsFrame(TagLib::Tag* tag);
			virtual ~LyricsFrame();

			bool map_tag_to_model(QString& model) override;
			bool map_model_to_tag(const QString& model) override;
    };

}

#endif // LYRICS_H
