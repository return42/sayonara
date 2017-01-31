#ifndef MP4_POPULARIMETERFRAME_H
#define MP4_POPULARIMETERFRAME_H

#include "Helper/Tagging/MP4/MP4Frame.h"
#include "Helper/Tagging/Models/Popularimeter.h"

namespace MP4
{
	class PopularimeterFrame :
			public MP4::MP4Frame<Models::Popularimeter>
	{
	public:
		PopularimeterFrame(TagLib::Tag* tag);
		~PopularimeterFrame();

		// AbstractFrame interface
	protected:
		bool map_tag_to_model(Models::Popularimeter& model) override;
		bool map_model_to_tag(const Models::Popularimeter& model) override;
	};
}

#endif // MP4_POPULARIMETERFRAME_H
