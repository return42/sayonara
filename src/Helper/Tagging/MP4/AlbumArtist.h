#ifndef MP4_ALBUMARTIST_H
#define MP4_ALBUMARTIST_H

#include "Helper/Tagging/MP4/MP4Frame.h"
#include <QString>

namespace MP4
{
	class AlbumArtistFrame :
			public MP4::MP4Frame<QString>
	{
	public:
		AlbumArtistFrame(TagLib::Tag* tag);
		~AlbumArtistFrame();

		// AbstractFrame interface
	protected:
		bool map_tag_to_model(QString& model) override;
		bool map_model_to_tag(const QString& model) override;
	};
}

#endif // MP4_ALBUMARTIST_H
