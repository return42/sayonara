#ifndef POPULARIMETERFRAME_H
#define POPULARIMETERFRAME_H

#include "Helper/Tagging/Xiph/AbstractFrame.h"
#include "Helper/Tagging/Models/Popularimeter.h"

/**
 * @ingroup Tagging
 */
namespace Xiph
{
	/**
	 * @brief The PopularimeterFrame class
	 * @ingroup Xiph
	 */
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
