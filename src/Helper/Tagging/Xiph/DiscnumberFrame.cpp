#include "DiscnumberFrame.h"
#include <QStringList>

Xiph::DiscnumberFrame::DiscnumberFrame(TagLib::Tag* tag) :
    Xiph::XiphFrame<Models::Discnumber>(tag, "DISCNUMBER")
{

}

Xiph::DiscnumberFrame::~DiscnumberFrame() {}

bool Xiph::DiscnumberFrame::map_tag_to_model(Models::Discnumber& model)
{
	TagLib::String str;
	bool success = value(str);
	if(!success){
		return false;
	}

	QString sval = cvt_string(str);
    QStringList lst = sval.split("/");
    if(lst.size() > 0){
		model.disc = lst[0].toInt();
    }

    if(lst.size() > 1){
		model.n_discs= lst[1].toInt();
    }

    return (lst.size() > 0);
}

bool Xiph::DiscnumberFrame::map_model_to_tag(const Models::Discnumber& model)
{
    QString str;
    str += QString::number(model.disc);
    str += "/";
    str += QString::number(model.n_discs);

	this->set_value(str);

    return true;
}

