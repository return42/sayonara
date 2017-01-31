#include "AlbumArtist.h"

MP4::AlbumArtistFrame::AlbumArtistFrame(TagLib::Tag* tag) :
	MP4::MP4Frame<QString>(tag, "aART") {}

MP4::AlbumArtistFrame::~AlbumArtistFrame() {}

bool MP4::AlbumArtistFrame::map_tag_to_model(QString& model)
{
	TagLib::MP4::Tag* tag = this->tag();
	const TagLib::MP4::ItemListMap& ilm = tag->itemListMap();

	auto it = find_key(ilm);
	if(it == ilm.end()){
		return false;
	}

	TagLib::MP4::Item item = it->second;
	if(!item.isValid()){
		return false;
	}

	TagLib::StringList items = item.toStringList();
	if(items.size() > 0){
		model = cvt_string( *items.begin() );
		return true;
	}

	return false;
}

bool MP4::AlbumArtistFrame::map_model_to_tag(const QString& model)
{
	TagLib::MP4::Tag* tag = this->tag();

	TagLib::String key_str = tag_key();
	TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::String str = cvt_string(model);
	TagLib::StringList strings;

	strings.append(str);
	ilm.insert(key_str, strings);
	return true;
}
