#include "AlbumArtist.h"
#include <QString>

Xiph::AlbumArtistFrame::AlbumArtistFrame(TagLib::Tag* tag) :
    Xiph::AbstractFrame<QString>(tag, "ALBUMARTIST")
{

}

Xiph::AlbumArtistFrame::~AlbumArtistFrame() {}

bool Xiph::AlbumArtistFrame::map_tag_to_model(const TagLib::String& value, QString& model)
{
    model = cvt_string(value);
    return true;
}

bool Xiph::AlbumArtistFrame::map_model_to_tag(const QString& model, TagLib::Ogg::XiphComment* tag)
{
    tag->addField( cvt_string(key()), cvt_string(model), true);
    return true;
}
