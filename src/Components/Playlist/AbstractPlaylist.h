/* Playlist.h */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "PlaylistDBInterface.h"
#include "Helper/typedefs.h"
#include "Helper/Playlist/PlaylistFwd.h"
#include "Helper/Playlist/PlaylistMode.h"
#include "Helper/Settings/SayonaraClass.h"
#include "Helper/Pimpl.h"

#include <QString>

class QStringList;
namespace SP
{
	template<typename T>
	class Set;
}

class MetaDataList;
class MetaData;
/**
 * @brief The Playlist class
 * @ingroup Playlists
 */
class AbstractPlaylist :
		public PlaylistDBInterface,
		protected SayonaraClass
{
	Q_OBJECT
	PIMPL(AbstractPlaylist)

	friend class PlaylistHandler;

signals:
	void sig_data_changed(int idx);

protected:
	virtual void play()=0;
	virtual void pause()=0;
	virtual void stop()=0;
	virtual void fwd()=0;
	virtual void bwd()=0;
    virtual void next()=0;
	virtual bool wake_up()=0;

	virtual int create_playlist(const MetaDataList& v_md)=0;
	virtual void replace_track(int idx, const MetaData& metadata);

	MetaDataList& metadata();
	MetaData& metadata(int idx);


public:
	explicit AbstractPlaylist(int idx, const QString& name=QString());
	virtual ~AbstractPlaylist();

	QStringList		toStringList() const;

	IdxList			find_tracks(int id) const;
	IdxList			find_tracks(const QString& filepath) const;
	int				current_track_index() const;
	bool			current_track(MetaData& metadata) const;
	int				playlist_index() const;
	void			set_playlist_index(int idx);
	void			set_playlist_mode(const Playlist::Mode& mode);
	uint64_t		running_time() const;
	Playlist::Mode	playlist_mode() const;

	virtual 
	Playlist::Type	type() const = 0;


	// from PlaylistDBInterface
	bool				is_empty() const override;
	int					count() const override;
	const MetaDataList&	playlist() const override;

	void				set_changed(bool b) override;
	bool				was_changed() const override;
	bool				is_storable() const override;

protected:
	void				set_storable(bool b);

public:
	const MetaData& operator[](int idx) const;
	const MetaData& at_const_ref(int idx) const;

	virtual void clear();

	virtual void move_tracks(const SP::Set<int>& indexes, int tgt);

	virtual void copy_tracks(const SP::Set<int>& indexes, int tgt);

	virtual void delete_tracks(const SP::Set<int>& indexes);

	virtual void insert_track(const MetaData& metadata, int tgt);
	virtual void insert_tracks(const MetaDataList& lst, int tgt);

	virtual void append_tracks(const MetaDataList& lst);

    virtual bool change_track(int idx);

public slots:
	virtual void metadata_deleted(const MetaDataList& v_md_deleted)=0;
	virtual void metadata_changed(const MetaDataList& v_md_old, const MetaDataList& v_md_new)=0;
	virtual void metadata_changed_single(const MetaData& metadata)=0;
	virtual void duration_changed(uint64_t duration)=0;


private slots:
    void _sl_playlist_mode_changed();
};

#endif // PLAYLIST_H
