#ifndef METADATALIST_H
#define METADATALIST_H

#include <QVector>
#include "MetaData.h"
#include <functional>

/**
 * @brief The MetaDataList class
 * @ingroup MetaDataHelper
 */
class MetaDataInfo;
class AlbumInfo;
class ArtistInfo;

class MetaDataList : public QVector<MetaData> {

private:
	int _cur_played_track;

public:

	MetaDataList();
	MetaDataList(const MetaDataList&);
	MetaDataList(int n_elems);

	virtual ~MetaDataList();

	void set_cur_play_track(int idx);
	void set_cur_play_track_idx_only(int idx);
	int get_cur_play_track() const;


	virtual bool contains(const MetaData& md) const;
	MetaDataList& remove_track(int idx);
	MetaDataList& remove_tracks(IdxList rows);

	MetaDataList& move_tracks(const IdxList& rows, int tgt_idx);
	MetaDataList& insert_tracks(const MetaDataList& v_md, int tgt_idx);
	MetaDataList& randomize();

	MetaDataList extract_tracks(std::function<bool (const MetaData& md)> func) const;
	MetaDataList extract_tracks(const IdxList& idx_list) const;

	IdxList findTracks(int id) const;
	IdxList findTracks(const QString&) const;

	QStringList toStringList() const;

	MetaDataInfo* get_info() const ;
	ArtistInfo* get_artist_info() const ;
	AlbumInfo* get_album_info() const;

	MetaDataList& operator <<(const MetaDataList& v_md);
	MetaDataList& operator <<(const MetaData& md);


};


#endif // METADATALIST_H
