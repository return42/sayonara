#include "CustomMimeData.h"

CustomMimeData::CustomMimeData() : QMimeData(), _v_md(0){
    _has_meta_data = false;
}

CustomMimeData::~CustomMimeData(){
}

void CustomMimeData::setMetaData(const MetaDataList& v_md){

    _v_md = v_md;
    _has_meta_data = (v_md.size() > 0);
}

int CustomMimeData::getMetaData(MetaDataList& v_md) const {

    if(!_has_meta_data) return 0;
    if(_v_md.size() == 0) return 0;
    v_md = _v_md;
    return _v_md.size();
}

bool CustomMimeData::hasMetaData() const {

    return _has_meta_data;
}
