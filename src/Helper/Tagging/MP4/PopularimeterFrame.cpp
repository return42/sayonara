#include "PopularimeterFrame.h"


MP4::PopularimeterFrame::PopularimeterFrame(TagLib::Tag* tag) :
	MP4::AbstractFrame<Models::Popularimeter>(tag, "rtng")
{

}

MP4::PopularimeterFrame::~PopularimeterFrame() {}

bool MP4::PopularimeterFrame::map_tag_to_model(TagLib::MP4::Tag* tag, Models::Popularimeter& model)
{
	TagLib::MP4::ItemListMap ilm = tag->itemListMap();
	TagLib::String key_str = cvt_string( key() );
	TagLib::MP4::Item item = ilm[key_str];


	if(item.isValid()){
		int rating = item.toInt();
		if(rating < 10){
			model.set_rating(rating);
		}

		else{
			model.set_rating_byte(rating);
		}

		return true;
	}

	return false;
}


bool MP4::PopularimeterFrame::map_model_to_tag(const Models::Popularimeter& model, TagLib::MP4::Tag* tag)
{
	TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::String key_str = cvt_string( key() );
	TagLib::MP4::Item item((int) model.get_rating());

	ilm.insert(key_str, item);

	return true;
}
