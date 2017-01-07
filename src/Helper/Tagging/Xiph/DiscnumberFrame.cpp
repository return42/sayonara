#include "DiscnumberFrame.h"
#include <QStringList>

Xiph::DiscnumberFrame::DiscnumberFrame(TagLib::Tag* tag) :
    Xiph::AbstractFrame<Models::Discnumber>(tag, "DISCNUMBER")
{

}

Xiph::DiscnumberFrame::~DiscnumberFrame() {}

bool Xiph::DiscnumberFrame::map_tag_to_model(const TagLib::String& value, Models::Discnumber& model)
{
    QString sval = cvt_string(value);
    QStringList lst = sval.split("/");
    if(lst.size() > 0){
	model.disc = lst[0].toInt();
    }

    if(lst.size() > 1){
	model.n_discs= lst[1].toInt();
    }

    return (lst.size() > 0);
}

bool Xiph::DiscnumberFrame::map_model_to_tag(const Models::Discnumber& model, TagLib::Ogg::XiphComment* tag)
{
    QString str;
    str += QString::number(model.disc);
    str += "/";
    str += QString::number(model.n_discs);

	tag->addField( cvt_string(key()), cvt_string(str), true );

    return true;
}

