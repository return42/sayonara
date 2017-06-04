#include "LyricsFrame.h"

Xiph::LyricsFrame::LyricsFrame(TagLib::Tag* tag) :
    Xiph::XiphFrame<QString>(tag, "LYRICS")
{}

Xiph::LyricsFrame::~LyricsFrame() {}

bool Xiph::LyricsFrame::map_tag_to_model(QString& model)
{
    TagLib::String str;
    bool success = value(str);
    if(success){
	    model = cvt_string(str);
    }

    return success;
}

bool Xiph::LyricsFrame::map_model_to_tag(const QString& model)
{
    set_value(model);
    return true;
}
