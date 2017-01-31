#include "PopularimeterFrame.h"


MP4::PopularimeterFrame::PopularimeterFrame(TagLib::Tag* tag) :
	MP4::MP4Frame<Models::Popularimeter>(tag, "rtng")
{

}

MP4::PopularimeterFrame::~PopularimeterFrame() {}

bool MP4::PopularimeterFrame::map_tag_to_model(Models::Popularimeter& model)
{
	TagLib::MP4::Tag* tag = this->tag();
	TagLib::MP4::ItemListMap ilm = tag->itemListMap();
	TagLib::MP4::Item item = ilm[tag_key()];

	if(item.isValid()){
		quint8 rating = (quint8) item.toByte();
		if(rating <= 5){
			model.set_rating(rating);
		}

		else{
			model.set_rating_byte(rating);
		}

		return true;
	}

	return false;
}


bool MP4::PopularimeterFrame::map_model_to_tag(const Models::Popularimeter& model)
{
	TagLib::MP4::Tag* tag = this->tag();
	TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::MP4::Item item((uchar) model.get_rating_byte());

	ilm.insert(tag_key(), item);

	return true;
}
