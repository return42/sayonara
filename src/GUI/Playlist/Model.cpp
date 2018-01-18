/* PlaylistItemModel.cpp */

/* Copyright (C) 2011-2017 Lucio Carreras
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


/*
* PlaylistItemModel.cpp
 *
 *  Created on: Apr 8, 2011
 *      Author: Lucio Carreras
 */

#include "Model.h"
#include "Components/Playlist/AbstractPlaylist.h"
#include "Components/Tagging/Editor.h"
#include "Components/Covers/CoverLocation.h"

#include "GUI/Utils/CustomMimeData.h"
#include "GUI/Utils/GuiUtils.h"

#include "Utils/MetaData/MetaDataList.h"
#include "Utils/Library/SearchMode.h"
#include "Utils/Utils.h"
#include "Utils/FileUtils.h"
#include "Utils/Set.h"
#include "Utils/globals.h"
#include "Utils/Language.h"
#include "Utils/Logger/Logger.h"
#include "Utils/Settings/Settings.h"

#include <QUrl>
#include <QPalette>
#include <QHash>
#include <QMainWindow>
#include <QPixmap>

enum class PlaylistSearchMode
{
	Artist,
	Album,
	Title,
	Jump
};

struct PlaylistItemModel::Private
{
	const static QChar album_search_prefix='%';
	const static QChar artist_search_prefix='$';
	const static QChar jump_prefix=':';

	QHash<AlbumId, QPixmap>	pms;
	int						old_row_count;
	PlaylistPtr				pl=nullptr;

	Private(PlaylistPtr pl) :
		old_row_count(0),
		pl(pl)
	{}

	PlaylistSearchMode get_search_mode(const QString& str)
	{
		if(str.startsWith(artist_search_prefix)){
			return PlaylistSearchMode::Artist;
		}
		else if(str.startsWith(album_search_prefix)){
			return PlaylistSearchMode::Album;
		}
		else if(str.startsWith(jump_prefix))
		{
			return PlaylistSearchMode::Jump;
		}

		return PlaylistSearchMode::Title;
	}

	QString remove_search_prefix(QString str, PlaylistSearchMode search_mode)
	{
		switch(search_mode)
		{
			case PlaylistSearchMode::Artist:
				str.remove(artist_search_prefix);
			case PlaylistSearchMode::Album:
				str.remove(album_search_prefix);
			case PlaylistSearchMode::Jump:
				str.remove(jump_prefix);
		}

		return str.trimmed();
	}

	QString converted_string(const MetaData& md, PlaylistSearchMode search_mode, ::Library::SearchModeMask smm)
	{
		QString str;
		switch(search_mode)
		{
			case PlaylistSearchMode::Artist:
				str = md.artist(); break;
			case PlaylistSearchMode::Album:
				str = md.album(); break;
			case PlaylistSearchMode::title:
				str = md.title(); break;
		}

		return Library::Util::convert_search_string(str, smm);
	}
};

PlaylistItemModel::PlaylistItemModel(PlaylistPtr pl, QObject* parent) :
	SearchableTableModel(parent)
{
	m = Pimpl::make<Private>(pl);

	connect(m->pl.get(), &Playlist::Base::sig_data_changed, this, &PlaylistItemModel::playlist_changed);

	playlist_changed(0);
}

PlaylistItemModel::~PlaylistItemModel() {}

int PlaylistItemModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m->pl->count();
}

int PlaylistItemModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return (int)(ColumnName::NumColumns);
}


QVariant PlaylistItemModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	int col = index.column();

	if (!index.isValid()) {
		return QVariant();
	}

	if ( !between(index.row(), m->pl->count())) {
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		switch(col)
		{
			case ColumnName::Cover:
				return QVariant();
			case ColumnName::TrackNumber:
				return QString("%1.").arg(row + 1);
			case ColumnName::Description:
				return QVariant();
			case ColumnName::Time:
			{
				auto l = m->pl->metadata(row).length_ms;
				return Util::cvt_ms_to_string(l, true, true, false);
			}
			default:
				return QVariant();
		}
	}

	else if (role == Qt::TextAlignmentRole)
	{
		switch(col)
		{
			case ColumnName::Cover:
				return QVariant();
			case ColumnName::TrackNumber:
				return QVariant(Qt::AlignRight | Qt::AlignVCenter);
			case ColumnName::Description:
				return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
			case ColumnName::Time:
				return QVariant(Qt::AlignRight | Qt::AlignVCenter);
			default:
				return QVariant();
		}
	}

	else if (role == Qt::BackgroundColorRole)
	{
		if(m->pl->current_track_index() == row)
		{
			QPalette palette = Gui::Util::main_window()->palette();
			QColor col_highlight = palette.color(QPalette::Active, QPalette::Highlight);
			col_highlight.setAlpha(80);
			return col_highlight;
		}
	}

	else if(role == Qt::FontRole)
	{
		Settings* s = Settings::instance();
		QFont f = Gui::Util::main_window()->font();
		int point_size = s->get(Set::PL_FontSize);
		if(point_size > 0){
			f.setPointSize(point_size);
		}

		if(col == ColumnName::TrackNumber)
		{
			// todo: read from settings
			f.setBold(true);
		}

		return f;
	}

	else if(role == Qt::DecorationRole)
	{
		if(col == ColumnName::Cover)
		{
			AlbumId album_id = m->pl->metadata(row).album_id;
			if(!m->pms.contains(album_id))
			{
				Cover::Location cl = Cover::Location::cover_location(m->pl->metadata(row));
				m->pms[album_id] = QPixmap(cl.preferred_path()).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}

			return m->pms[album_id];
		}
	}

	return QVariant();
}


Qt::ItemFlags PlaylistItemModel::flags(const QModelIndex &index) const
{
	int row = index.row();
	if (!index.isValid()){
		return (Qt::ItemIsEnabled);
	}


	if( row >= 0 && row < m->pl->count())
	{
		const MetaData& md = metadata(row);
		if(md.is_disabled){
			return Qt::NoItemFlags;
		}
	}

	return (QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
}

void PlaylistItemModel::clear()
{
	m->pl->clear();
}

void PlaylistItemModel::remove_rows(const IndexSet& indexes)
{
	m->pl->remove_tracks(indexes);
}


void PlaylistItemModel::move_rows(const IndexSet& indexes, int target_index)
{
	m->pl->move_tracks(indexes, target_index);
}


IndexSet PlaylistItemModel::move_rows_up(const IndexSet& indexes)
{
	int min_row = *(std::min_element(indexes.begin(), indexes.end()));
	if(min_row <= 0){
		return IndexSet();
	}

	move_rows(indexes, min_row - 1);

	IndexSet new_indexes;
	for(int i=0; i<indexes.count(); i++)
	{
		new_indexes.insert(i + min_row - 1);
	}

	return new_indexes;
}


IndexSet PlaylistItemModel::move_rows_down(const IndexSet& indexes)
{
	auto min_max = std::minmax_element(indexes.begin(), indexes.end());
	int min_row = *(min_max.first);
	int max_row = *(min_max.second);

	if(max_row >= rowCount() - 1){
		return IndexSet();
	}

	IndexSet new_indexes;
	move_rows(indexes, max_row + 2);

	for(int i=0; i<indexes.count(); i++)
	{
		new_indexes.insert(i + min_row + 1);
	}

	return new_indexes;
}

void PlaylistItemModel::copy_rows(const IndexSet& indexes, int target_index)
{
	m->pl->copy_tracks(indexes, target_index);
}

void PlaylistItemModel::change_rating(const IndexSet& indexes, Rating rating)
{
	MetaDataList v_md;
	v_md.reserve(indexes.size());
	for(auto idx : indexes)
	{
		v_md << m->pl->metadata(idx);
	}

	Tagging::Editor* te = new Tagging::Editor(v_md);

	for(int i=0; i<v_md.count(); i++)
	{
		MetaData md	= v_md[i];
		md.rating = rating;
		te->update_track(i, md);
	}

	te->commit();

	connect(te, &QThread::finished, te, &Tagging::Editor::deleteLater);
}


int PlaylistItemModel::current_track() const
{
	return m->pl->current_track_index();
}


void PlaylistItemModel::set_current_track(int row)
{
	m->pl->change_track(row);
}

const MetaData& PlaylistItemModel::metadata(int row) const
{
	return m->pl->metadata(row);
}


MetaDataList PlaylistItemModel::metadata(const IndexSet &rows) const
{
	MetaDataList v_md;
	v_md.reserve(rows.size());

	for(int row : rows)
	{
		v_md << m->pl->metadata(row);
	}

	return v_md;
}



QModelIndex PlaylistItemModel::getPrevRowIndexOf(const QString& substr, int row, const QModelIndex& parent)
{
	return getRowIndexOf(substr, row, false);
}

QModelIndex PlaylistItemModel::getNextRowIndexOf(const QString& substr, int row, const QModelIndex &parent)
{
	return getRowIndexOf(substr, row, true);
}

QModelIndex PlaylistItemModel::getRowIndexOf(const QString& substr, int row, bool is_forward)
{
	int len = m->pl->count();
	if(row >= len) {
		row = len - 1;
	}

	PlaylistSearchMode plsm = m->get_search_mode(substr);
	QString pure_search_string = m->remove_search_prefix(substr, search_mode);

	if(plsm == PlaylistSearchMode::Jump)
	{
		bool ok;
		int line = pure_search_string.toInt(&ok);
		if(ok && len > line) {
			return this->index(line, 0);
		}
	}

	else
	{
		for(int i=0; i<len; i++)
		{
			int row_idx;
			if(is_forward){
				row_idx = (i + row) % len;
			}

			else {
				if(row - i < 0) {
					row = len - 1;
				}

				row_idx = (row - i) % len;
			}

			QString str = m->converted_string(m->pl->metadata(row_idx), plsm, search_mode());
			if(str.contains(pure_search_string))
			{
				return this->index(row_idx, 0);
			}
		}
	}

	return index(-1, -1);
}

using ExtraTriggerMap=SearchableModelInterface::ExtraTriggerMap;
ExtraTriggerMap PlaylistItemModel::getExtraTriggers()
{
	ExtraTriggerMap map;

	map.insert(artist_search_prefix, Lang::get(Lang::Artist));
	map.insert(album_search_prefix, Lang::get(Lang::Album));
	map.insert(jump_prefix, tr("Goto row"));

	return map;
}


QMimeData* PlaylistItemModel::mimeData(const QModelIndexList& indexes) const
{
	if(indexes.isEmpty()){
		return nullptr;
	}

	CustomMimeData* mimedata = new CustomMimeData(this);

	MetaDataList v_md;
	v_md.reserve(indexes.size());

	SP::Set<int> rows;
	for(auto idx : indexes) {
		rows.insert(idx.row());
	}

	for(int row : rows)
	{
		if(row >= m->pl->count()){
			continue;
		}

		v_md << m->pl->metadata(row);
	}

	if(v_md.empty()){
		return nullptr;
	}

	mimedata->set_metadata(v_md);
	mimedata->set_playlist_source_index(m->pl->index());

	return mimedata;
}

bool PlaylistItemModel::has_local_media(const IndexSet& rows) const
{
	const  MetaDataList& tracks = m->pl->playlist();

	for(int row : rows){
		if(!Util::File::is_www(tracks[row].filepath())){
			return true;
		}
	}

	return false;
}

void PlaylistItemModel::playlist_changed(int pl_idx)
{
	Q_UNUSED(pl_idx)

	if(m->old_row_count > m->pl->count())
	{
		beginRemoveRows(QModelIndex(), m->pl->count(), m->old_row_count - 1);
		endRemoveRows();
	}

	else if(m->pl->count() > m->old_row_count){
		beginInsertRows(QModelIndex(), m->old_row_count, m->pl->count() - 1);
		endInsertRows();
	}

	if(m->pl->count() == 0){
		beginResetModel();
		endResetModel();
	}

	m->old_row_count = m->pl->count();

	emit sig_data_ready();
}

