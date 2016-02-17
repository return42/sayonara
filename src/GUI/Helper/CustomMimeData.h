/* CustomMimeData.h */

/* Copyright (C) 2013  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef _CUSTOMMIMEDATA_H
#define _CUSTOMMIMEDATA_H

#include "Helper/MetaData/MetaDataList.h"
#include <QMimeData>
#include <QUrl>


class CustomMimeData : public QMimeData {

private:
	MetaDataList _v_md;
    bool _has_meta_data;

public:
	CustomMimeData() : QMimeData(), _v_md(0){
        _has_meta_data = false;
    }

    virtual ~CustomMimeData(){
    }

    void setMetaData(const MetaDataList& v_md){

		_v_md = v_md;
        _has_meta_data = (v_md.size() > 0);
	}

	int getMetaData(MetaDataList& v_md) const {

        if(!_has_meta_data) return 0;
        if(_v_md.size() == 0) return 0;
		v_md = _v_md;
        return _v_md.size();
	}


    bool hasMetaData() const {

        return _has_meta_data;
    }
};


#endif
