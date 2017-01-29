#ifndef MP4COVER_H
#define MP4COVER_H

#include "AbstractFrame.h"
#include "Helper/Tagging/Models/Cover.h"
#include <taglib/mp4coverart.h>
#include <taglib/tag.h>


namespace MP4
{
	class CoverFrame :
			public AbstractFrame<Models::Cover>
	{
	public:
		explicit CoverFrame(TagLib::Tag* tag);
		~CoverFrame();


		// AbstractFrame interface
	protected:
		bool map_tag_to_model(TagLib::MP4::Tag* value, Models::Cover& model) override;
		bool map_model_to_tag(const Models::Cover& model, TagLib::MP4::Tag* tag) override;
	};
}
#endif // MP4COVER_H
