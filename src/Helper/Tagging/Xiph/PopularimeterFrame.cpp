#include "PopularimeterFrame.h"

Xiph::PopularimeterFrame::PopularimeterFrame(TagLib::Tag* tag) :
	XiphFrame<Models::Popularimeter>(tag, "RATING")
{

}

Xiph::PopularimeterFrame::~PopularimeterFrame() {}

bool Xiph::PopularimeterFrame::map_tag_to_model(Models::Popularimeter& model)
{
	TagLib::String str;
	bool success = this->value(str);
	if(!success){
		return false;
	}

	quint8 rating = (quint8) cvt_string(str).toInt();
	if(rating < 10){
		model.set_rating(rating);
	}

	else{
		model.set_rating_byte(rating);
	}

	return true;
}

bool Xiph::PopularimeterFrame::map_model_to_tag(const Models::Popularimeter& model)
{
	quint8 rating = model.get_rating();
	set_value(QString::number(rating));
	return true;
}
