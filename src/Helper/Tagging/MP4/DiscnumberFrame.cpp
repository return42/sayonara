#include "DiscnumberFrame.h"
#include <taglib/mp4item.h>

MP4::DiscnumberFrame::DiscnumberFrame(TagLib::Tag* tag) :
    MP4::AbstractFrame<Models::Discnumber>(tag, "disk")
{

}

MP4::DiscnumberFrame::~DiscnumberFrame() {}

bool MP4::DiscnumberFrame::map_tag_to_model(TagLib::MP4::Tag* tag, Models::Discnumber& model)
{
	TagLib::MP4::ItemListMap ilm = tag->itemListMap();
	TagLib::String key_str = cvt_string( key() );
	TagLib::MP4::Item item = ilm[key_str];

	if(item.isValid()){
		TagLib::MP4::Item::IntPair p = item.toIntPair();
		model.disc = p.first;
		model.n_discs = p.second;
		return true;
	}

	return false;
}

bool MP4::DiscnumberFrame::map_model_to_tag(const Models::Discnumber& model, TagLib::MP4::Tag* tag)
{
	TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::String key_str = cvt_string( key() );
	TagLib::MP4::Item item(model.disc, model.n_discs);

	auto it = ilm.find(key_str);
	while(it != ilm.end()){
		ilm.erase(it);
		it = ilm.find(key_str);
	}

	ilm.insert(key_str, item);

    return true;
}

