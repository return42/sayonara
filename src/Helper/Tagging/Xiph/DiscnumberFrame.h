#ifndef DISCNUMBERXIPHFRAME_H
#define DISCNUMBERXIPHFRAME_H

#include "Helper/Tagging/Models/Discnumber.h"
#include "XiphFrame.h"

/**
 * @ingroup Tagging
 */
namespace Xiph
{
	/**
	 * @brief The DiscnumberFrame class
	 * @ingroup Xiph
	 */
	class DiscnumberFrame :
		public Xiph::XiphFrame<Models::Discnumber>
	{
	public:
		DiscnumberFrame(TagLib::Tag* tag);
		~DiscnumberFrame();

	protected:
		bool map_tag_to_model(Models::Discnumber& model);
		bool map_model_to_tag(const Models::Discnumber& model);
	};
}

#endif // DISCNUMBERFRAME_H
