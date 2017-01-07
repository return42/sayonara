#include "PopularimeterFrame.h"

Xiph::PopularimeterFrame::PopularimeterFrame(TagLib::Tag* tag) :
	AbstractFrame<Models::Popularimeter>(tag, "RATING")
{

}

Xiph::PopularimeterFrame::~PopularimeterFrame() {}

bool Xiph::PopularimeterFrame::map_tag_to_model(const TagLib::String& value, Models::Popularimeter& model)
{
	QString str = cvt_string(value);
	if(str.isEmpty()){
		return false;
	}

	quint8 rating = (quint8) str.toInt();
	if(rating < 10){
		model.set_sayonara_rating(rating);
	}

	else{
		model.rating_byte = rating;
	}

	return true;
}

bool Xiph::PopularimeterFrame::map_model_to_tag(const Models::Popularimeter& model, TagLib::Ogg::XiphComment* tag)
{
	quint8 rating = model.get_sayonara_rating();
	QString str = QString::number(rating);
	tag->addField( cvt_string(key()), cvt_string(str), true );

	return true;
}
