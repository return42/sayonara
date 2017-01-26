#include "AlbumArtist.h"

MP4::AlbumArtistFrame::AlbumArtistFrame(TagLib::Tag* tag) :
	MP4::AbstractFrame<QString>(tag, "aART")
{

}

MP4::AlbumArtistFrame::~AlbumArtistFrame() {}

bool MP4::AlbumArtistFrame::map_tag_to_model(TagLib::MP4::Tag* tag, QString& model)
{
	const TagLib::MP4::ItemListMap& ilm = tag->itemListMap();

	auto it = find_key(ilm);
	if(it == ilm.end()){
		sp_log(Log::Debug) << "Album artist not found 1";
		return false;
	}

	TagLib::MP4::Item item = it->second;
	if(!item.isValid()){
		sp_log(Log::Debug) << "Album artist not found 2";
		return false;
	}

	TagLib::StringList items = item.toStringList();
	if(items.size() > 0){
		model = cvt_string( *items.begin() );
		sp_log(Log::Debug) << "Album artist = " << model;
		return true;
	}

	sp_log(Log::Debug) << "Album artist not found 3";
	return false;
}

bool MP4::AlbumArtistFrame::map_model_to_tag(const QString& model, TagLib::MP4::Tag* tag)
{
	TagLib::String key_str = cvt_string( key() );
	TagLib::MP4::ItemListMap& ilm = tag->itemListMap();
	TagLib::String str = cvt_string(model);
	TagLib::StringList strings;
	strings.append(str);
	ilm.insert(key_str, strings);
	return true;
}
