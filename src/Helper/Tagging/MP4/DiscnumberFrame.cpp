#include "DiscnumberFrame.h"
#include <taglib/mp4item.h>

MP4::DiscnumberFrame::DiscnumberFrame(TagLib::Tag* tag) :
    MP4::MP4Frame<Models::Discnumber>(tag, "disk")
{

}

MP4::DiscnumberFrame::~DiscnumberFrame() {}

bool MP4::DiscnumberFrame::map_tag_to_model(Models::Discnumber& model)
{
	TagLib::MP4::Tag* tag = this->tag();

	TagLib::MP4::ItemListMap ilm = tag->itemListMap();
	TagLib::MP4::Item item = ilm[tag_key()];

	if(item.isValid()){
		TagLib::MP4::Item::IntPair p = item.toIntPair();
		model.disc = p.first;
		model.n_discs = p.second;
		return true;
	}

	return false;
}

bool MP4::DiscnumberFrame::map_model_to_tag(const Models::Discnumber& model)
{
	TagLib::MP4::ItemListMap& ilm = this->tag()->itemListMap();
	TagLib::MP4::Item item(model.disc, model.n_discs);
	TagLib::String key_str = tag_key();

	auto it = ilm.find(key_str);
	while(it != ilm.end()){
		ilm.erase(it);
		it = ilm.find(key_str);
	}

	ilm.insert(key_str, item);

    return true;
}

