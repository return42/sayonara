#ifndef ALBUMARTIST_H
#define ALBUMARTIST_H

#include "AbstractFrame.h"

namespace ID3v2Frame
{
	class AlbumArtistFrame :
			public AbstractFrame<QString, TagLib::ID3v2::TextIdentificationFrame>
	{

		public:
			explicit AlbumArtistFrame(TagLib::FileRef* file_ref);
			virtual ~AlbumArtistFrame();

		protected:
			TagLib::ID3v2::Frame* create_id3v2_frame() override;

			void map_model_to_frame() override;
			void map_frame_to_model() override;
	};
}

#endif // ALBUMARTIST_H
