#ifndef POPULARIMETERFRAME_H
#define POPULARIMETERFRAME_H

#include "Tagging/Xiph/AbstractFrame.h"
#include "Tagging/Models/Popularimeter.h"

namespace Xiph
{
	class PopularimeterFrame : public AbstractFrame<Models::Popularimeter>
	{
	public:
		PopularimeterFrame(TagLib::Tag* tag);
		virtual ~PopularimeterFrame();

	protected:
		bool map_tag_to_model(const TagLib::String& value, Models::Popularimeter& model);
		bool map_model_to_tag(const Models::Popularimeter& model, TagLib::Ogg::XiphComment* tag);
	};
}

#endif // POPULARIMETERFRAME_H
