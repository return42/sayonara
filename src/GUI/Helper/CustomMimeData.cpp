#include "CustomMimeData.h"

CustomMimeData::CustomMimeData() :
	QMimeData(),
	_v_md(0)
{

}

CustomMimeData::~CustomMimeData(){
}

void CustomMimeData::setMetaData(const MetaDataList& v_md){

    _v_md = v_md;
}

MetaDataList CustomMimeData::getMetaData() const
{
	return _v_md;
}

bool CustomMimeData::hasMetaData() const {

	return (_v_md.size() > 0);
}
