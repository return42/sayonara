#ifndef XIPH_ALBUMARTIST_H_
#define XIPH_ALBUMARTIST_H_

#include "AbstractFrame.h"
#include <QString>

namespace Xiph
{
    class AlbumArtistFrame :
	    public Xiph::AbstractFrame<QString>
    {
	public:
	    AlbumArtistFrame(TagLib::Tag* tag);
	    virtual ~AlbumArtistFrame();

	protected:
	    bool map_tag_to_model(const TagLib::String& value, QString& model);
	    bool map_model_to_tag(const QString& model, TagLib::Ogg::XiphComment* tag);
    };
}

#endif // ALBUMARTIST_H
