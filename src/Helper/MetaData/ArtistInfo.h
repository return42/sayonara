#ifndef ARTISTINFO_H
#define ARTISTINFO_H

#include "MetaDataInfo.h"

/**
 * @brief The ArtistInfo class
 * @ingroup MetaDataHelper
 */
class MetaDataList;
class ArtistInfo : public MetaDataInfo {

	Q_OBJECT

private:
	void set_cover_location() override;
	void set_subheader() override;
	void set_header() override;

	void calc_similar_artists(Artist& artist);

	QString get_sim_artist_string(QString key) const;

public:
	ArtistInfo(const MetaDataList*);
	virtual ~ArtistInfo();

	QString get_cover_album() const override;
	QString get_additional_info_as_string() const override;
};

#endif // ARTISTINFO_H

