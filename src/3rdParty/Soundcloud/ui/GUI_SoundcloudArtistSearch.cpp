/* GUI_SoundcloudArtistSearch.cpp */

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

#include "GUI_SoundcloudArtistSearch.h"
#include "3rdParty/Soundcloud/src/SoundcloudLibrary.h"
#include "3rdParty/Soundcloud/src/SoundcloudDataFetcher.h"
#include "3rdParty/Soundcloud/ui_GUI_SoundcloudArtistSearch.h"

#include "Helper/globals.h"
#include "Helper/MetaData/Artist.h"

struct GUI_SoundcloudArtistSearch::Private
{
	SoundcloudLibrary*		library=nullptr;
	SoundcloudDataFetcher*	fetcher=nullptr;

	MetaDataList		v_md;
	AlbumList			albums;
	ArtistList			searched_artists;
	ArtistList			chosen_artists;
	quint64				cur_artist_sc_id;
};

GUI_SoundcloudArtistSearch::GUI_SoundcloudArtistSearch(SoundcloudLibrary* library, QWidget *parent) :
	SayonaraDialog(parent)
{
	ui = new Ui::GUI_SoundcloudArtistSearch();
	ui->setupUi(this);

	_m = Pimpl::make<GUI_SoundcloudArtistSearch::Private>();
	_m->library = library;
	_m->fetcher = new SoundcloudDataFetcher(this);

	connect(ui->btn_search, &QPushButton::clicked, this, &GUI_SoundcloudArtistSearch::search_clicked);
	connect(ui->btn_add, &QPushButton::clicked, this, &GUI_SoundcloudArtistSearch::add_clicked);
	connect(ui->btn_cancel, &QPushButton::clicked, this, &GUI_SoundcloudArtistSearch::close);
	connect(ui->btn_clear, &QPushButton::clicked, this, &GUI_SoundcloudArtistSearch::clear_clicked);

	connect(ui->list_artists, &QListWidget::currentRowChanged, this, &GUI_SoundcloudArtistSearch::artist_selected);

	connect(_m->fetcher, &SoundcloudDataFetcher::sig_artists_fetched, this, &GUI_SoundcloudArtistSearch::artists_fetched);
	connect(_m->fetcher, &SoundcloudDataFetcher::sig_ext_artists_fetched, this, &GUI_SoundcloudArtistSearch::artists_ext_fetched);
	connect(_m->fetcher, &SoundcloudDataFetcher::sig_playlists_fetched, this, &GUI_SoundcloudArtistSearch::albums_fetched);
	connect(_m->fetcher, &SoundcloudDataFetcher::sig_tracks_fetched, this, &GUI_SoundcloudArtistSearch::tracks_fetched);

	clear_clicked();
}

GUI_SoundcloudArtistSearch::~GUI_SoundcloudArtistSearch() {}

void GUI_SoundcloudArtistSearch::search_clicked()
{
	QString text = ui->le_search->text();
	clear_clicked();

	ui->le_search->setText(text);

	if(text.size() <= 3){
		ui->lab_status->setText(tr("Query too short"));
	}

	set_playlist_label(-1);
	set_tracks_label(-1);

	_m->fetcher->search_artists(text);
}

void GUI_SoundcloudArtistSearch::clear_clicked()
{
	ui->list_artists->clear();
	ui->list_playlists->clear();
	ui->list_tracks->clear();
	ui->le_search->clear();
	ui->lab_status->clear();
	ui->lab_n_artists->clear();
	ui->btn_add->setEnabled(false);

	set_playlist_label(-1);
	set_tracks_label(-1);

	_m->searched_artists.clear();
	_m->chosen_artists.clear();
	_m->v_md.clear();
	_m->albums.clear();
}

void GUI_SoundcloudArtistSearch::add_clicked()
{
	if( _m->v_md.size() > 0 &&
		_m->chosen_artists.size() > 0 &&
		_m->albums.size() > 0)
	{
		_m->library->insert_tracks(_m->v_md, _m->chosen_artists, _m->albums);
		close();
	}
}

void GUI_SoundcloudArtistSearch::close_clicked()
{
	close();
}


void GUI_SoundcloudArtistSearch::artist_selected(int idx)
{
	ui->list_playlists->clear();
	ui->list_tracks->clear();

	set_playlist_label(-1);
	set_tracks_label(-1);

	_m->v_md.clear();
	_m->albums.clear();

	if(!between(idx, _m->searched_artists)) {
		return;
	}

	_m->cur_artist_sc_id = _m->searched_artists[idx].id;

	_m->chosen_artists.clear();

	_m->fetcher->get_tracks_by_artist(_m->cur_artist_sc_id);
}

void GUI_SoundcloudArtistSearch::language_changed()
{
	ui->retranslateUi(this);
}


void GUI_SoundcloudArtistSearch::artists_fetched(const ArtistList& artists)
{
	ui->list_artists->clear();
	_m->searched_artists.clear();

	if(artists.size() == 0){
		ui->lab_status->setText(tr("No artists found"));
		return;
	}

	else{
		ui->lab_n_artists->setText( tr("Found %1 artists").arg(artists.size()) );
		for(const Artist& artist: artists){
			ui->list_artists->addItem(artist.name);
		}

		_m->searched_artists = artists;
	}
}

void GUI_SoundcloudArtistSearch::artists_ext_fetched(const ArtistList &artists)
{
	_m->chosen_artists = artists;
}


void GUI_SoundcloudArtistSearch::albums_fetched(const AlbumList& albums)
{
	ui->list_playlists->clear();

	for(const Album& album : albums){
		ui->list_playlists->addItem(album.name);
	}

	_m->albums = albums;

	set_playlist_label(albums.size());
}


void GUI_SoundcloudArtistSearch::tracks_fetched(const MetaDataList& v_md)
{
	ui->list_tracks->clear();

	for(const MetaData& md : v_md){
		ui->list_tracks->addItem(md.title);
	}

	_m->v_md = v_md;

	ui->btn_add->setEnabled(v_md.size() > 0);

	set_tracks_label(v_md.size());
}


void GUI_SoundcloudArtistSearch::set_tracks_label(int n_tracks)
{
	if(n_tracks >= 0){
		ui->lab_n_tracks->setText( tr("%1 tracks found").arg(n_tracks) );
	}

	ui->lab_n_tracks->setVisible(n_tracks >= 0);
}

void GUI_SoundcloudArtistSearch::set_playlist_label(int n_playlists)
{
	if(n_playlists >= 0){
		ui->lab_n_playlists->setText( tr("%1 playlists found").arg(n_playlists) );
	}

	ui->lab_n_playlists->setVisible(n_playlists >= 0);
}

