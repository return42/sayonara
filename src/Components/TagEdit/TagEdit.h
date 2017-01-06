/* TagEdit.h */

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


#ifndef TAGEDIT_H
#define TAGEDIT_H

#include <QThread>
#include <QImage>
#include <QStringList>

#include "Helper/Pimpl.h"

class MetaDataList;
class MetaData;
/**
 * @brief The TagEdit class
 * Metadata has to be added using the set_metadata(const MetaDataList&) method.
 * Use update_track(int idx, const MetaData& md) to stage the changes you made
 * to the track. commit() starts the thread and writes changes to HDD and the
 * database. When finished the finished() signal is emitted.
 * @ingroup Tagging
 */

class TagEdit :
		public QThread
{
	Q_OBJECT

	PIMPL(TagEdit)

signals:
	void sig_progress(int);
	void sig_metadata_received(const MetaDataList& v_md);


public:
	explicit TagEdit(QObject* parent=nullptr);
	explicit TagEdit(const MetaDataList& v_md, QObject* parent=nullptr);
	virtual ~TagEdit();


	/**
	 * @brief undo changes for a specific track
	 * @param idx track index
	 */
	void undo(int idx);

	/**
	 * @brief undo changes for all tracks
	 */
	void undo_all();


	/**
	 * @brief get the (changed) metadata for a specific index
	 * @param idx track index
	 * @return MetaData object
	 */
	const MetaData& get_metadata(int idx) const;


	/**
	 * @brief get all (changed) metadata
	 * @return MetaDataList object
	 */
	const MetaDataList& get_all_metadata() const;


	/**
	 * @brief Add a genre to all (changed) metdata
	 * @param genre the genre name
	 */
	void add_genre_to_metadata(const QString& genre);


	/**
	 * @brief gets the number of tracks
	 * @return number of tracks
	 */
	int get_n_tracks() const;


	/**
	 * @brief writes changes to (changed) metadata for a specific track
	 * @param idx track index
	 * @param md new MetaData replacing the old track
	 */
	void update_track(int idx, const MetaData& md);

	/**
	 * @brief update the cover for a specific track.
	 * @param idx track index
	 * @param cover new cover image
	 */
	void update_cover(int idx, const QImage& cover);

	/**
	 * @brief remove_cover for a specific track
	 * @param idx track index
	 */
//	void remove_cover(int idx);

	/**
	 * @brief does the user want to replace/add a cover
	 * @param idx track index
	 * @return false, if no new alternative cover is desired
	 */
	bool has_cover_replacement(int idx) const;

	/**
	 * @brief checks, if the tracks' tag is an id3v2 tag
	 * @param idx track index
	 * @return
	 */
	bool is_id3v2_tag(int idx) const;


	/**
	 * @brief initializes the TagEdit object with a MetaDataList
	 * @param v_md new MetaDataList
	 */
	void set_metadata(const MetaDataList& v_md);


public slots:

	/**
	 * @brief Commits changes to db
	 */
	void commit();


private:

	/**
	 * @brief checks for new artists and albums
	 * @param new_artists returns new artists
	 * @param new_albums return new albums
	 */
	void check_for_new_artists_and_albums(QStringList& new_artists, QStringList& new_albums);

	/**
	 * @brief inserts new artists into db
	 * @param artists list of new artists
	 */
	void insert_new_artists(const QStringList& artists);

	/**
	 * @brief inserts new albums into db
	 * @param albums list of new albums
	 */
	void insert_new_albums(const QStringList& albums);


	/**
	 * @brief applies the new artists and albums to the original metadata
	 */
	void apply_artists_and_albums_to_md();

	void run() override;

private slots:
	void thread_finished();
};

#endif // TAGEDIT_H
