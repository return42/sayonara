#ifndef MP4_DISCNUMBERFRAME_H
#define MP4_DISCNUMBERFRAME_H

#include "Helper/Tagging/Models/Discnumber.h"
#include "Helper/Tagging/MP4/AbstractFrame.h"

/**
 * @ingroup Tagging
 */
namespace MP4
{
	/**
	 * @brief The DiscnumberFrame class
	 * @ingroup Xiph
	 */
	class DiscnumberFrame :
		public MP4::AbstractFrame<Models::Discnumber>
	{
	public:
		DiscnumberFrame(TagLib::Tag* tag);
		~DiscnumberFrame();

	protected:
		bool map_tag_to_model(TagLib::MP4::Tag* tag, Models::Discnumber& model);
		bool map_model_to_tag(const Models::Discnumber& model, TagLib::MP4::Tag* tag);
	};
}

#endif // MP4_DISCNUMBERXIPHFRAME_H
