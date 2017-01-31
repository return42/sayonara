#ifndef POPULARIMETERFRAME_H
#define POPULARIMETERFRAME_H

#include "Helper/Tagging/Xiph/XiphFrame.h"
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
	class PopularimeterFrame :
			public XiphFrame<Models::Popularimeter>
	{
	public:
		PopularimeterFrame(TagLib::Tag* tag);
		~PopularimeterFrame();

	protected:
		bool map_tag_to_model(Models::Popularimeter& model) override;
		bool map_model_to_tag(const Models::Popularimeter& model) override;
	};
}

#endif // POPULARIMETERFRAME_H
