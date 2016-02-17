#ifndef ALBUMINFO_H
#define ALBUMINFO_H

#include "MetaDataInfo.h"

/**
 * @brief The AlbumInfo class
 * @ingroup MetaDataHelper
 */
class AlbumInfo : public MetaDataInfo {

	Q_OBJECT

private:
	void set_cover_location();
	void set_subheader();
	void set_header();

public:
	AlbumInfo(const MetaDataList*);
	virtual ~AlbumInfo();
};


#endif // ALBUMINFO_H

