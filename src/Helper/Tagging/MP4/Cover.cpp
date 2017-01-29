#include "Cover.h"

MP4::CoverFrame::CoverFrame(TagLib::Tag* tag) :
	AbstractFrame<Models::Cover>(tag, "covr")
{

}

MP4::CoverFrame::~CoverFrame()
{

}


bool MP4::CoverFrame::map_tag_to_model(TagLib::MP4::Tag* tag, Models::Cover& model)
{
	const TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::MP4::ItemListMap::ConstIterator it = find_key(ilm);
	if(it == ilm.end()){
		return false;
	}

	TagLib::MP4::Item item = it->second;
	if(!item.isValid()){
		return false;
	}

	TagLib::MP4::CoverArtList arts = item.toCoverArtList();
	if(arts.isEmpty()){
		return  false;
	}

	TagLib::MP4::CoverArt art = arts[0];
	model.image_data = QByteArray(art.data().data(), art.data().size());

	return true;
}

bool MP4::CoverFrame::map_model_to_tag(const Models::Cover& model, TagLib::MP4::Tag* tag)
{
	const QByteArray& image_data = model.image_data;
	TagLib::ByteVector taglib_data;

	taglib_data.setData(image_data.data(), image_data.size());

	TagLib::MP4::CoverArt::Format format;
	switch(model.get_mime_type()){
		case Models::Cover::MimeType::PNG:
			format = TagLib::MP4::CoverArt::PNG;
			break;
		case Models::Cover::MimeType::JPEG:
			format = TagLib::MP4::CoverArt::JPEG;
			break;
		default:
			return false;
	}

	TagLib::MP4::CoverArt art(format, taglib_data);
	TagLib::MP4::CoverArtList arts;
	arts.append(art);
	TagLib::MP4::Item item(arts);
	TagLib::String tag_str = cvt_string(key());

	tag->itemListMap().insert(tag_str, item);

	return true;
}
