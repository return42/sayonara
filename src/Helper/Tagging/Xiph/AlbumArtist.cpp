#include "AlbumArtist.h"
#include <QString>

Xiph::AlbumArtistFrame::AlbumArtistFrame(TagLib::Tag* tag) :
	Xiph::XiphFrame<QString>(tag, "ALBUMARTIST") {}

Xiph::AlbumArtistFrame::~AlbumArtistFrame() {}

bool Xiph::AlbumArtistFrame::map_tag_to_model(QString& model)
{
	TagLib::String str;
	bool success = value(str);
	if(success){
		model = cvt_string(str);
	}

	return success;
}

bool Xiph::AlbumArtistFrame::map_model_to_tag(const QString& model)
{
	set_value(model);
    return true;
}
