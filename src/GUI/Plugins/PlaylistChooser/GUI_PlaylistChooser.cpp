/* GUI_PlaylistChooser.cpp */

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

#include "GUI_PlaylistChooser.h"
#include "GUI_TargetPlaylistDialog.h"
#include "GUI/Plugins/PlaylistChooser/ui_GUI_PlaylistChooser.h"

#include "Components/PlaylistChooser/PlaylistChooser.h"

#include "Utils/Message/Message.h"

#include "Utils/Utils.h"
#include "Utils/Playlist/CustomPlaylist.h"
#include "Utils/Language.h"
#include "Utils/Settings/Settings.h"
#include "Utils/FileUtils.h"

#include <QDir>
#include <QFileDialog>

#include <algorithm>

struct GUI_PlaylistChooser::Private
{
    PlaylistChooser*	playlist_chooser=nullptr;

    Private() :
        playlist_chooser(new PlaylistChooser())
    {}

    ~Private()
    {
        delete playlist_chooser; playlist_chooser = nullptr;
    }
};

GUI_PlaylistChooser::GUI_PlaylistChooser(QWidget *parent) :
    PlayerPluginInterface(parent)
{
    m = Pimpl::make<Private>();
}


GUI_PlaylistChooser::~GUI_PlaylistChooser()
{
    if(ui){
        delete ui; ui=nullptr;
    }
}


void GUI_PlaylistChooser::init_ui()
{
    setup_parent(this, &ui);

    connect(ui->combo_playlistchooser, combo_activated_int, this, &GUI_PlaylistChooser::playlist_selected);
    connect(m->playlist_chooser, &PlaylistChooser::sig_all_playlists_loaded,
            this, &GUI_PlaylistChooser::all_playlists_fetched);

    m->playlist_chooser->load_all_playlists();
}


void GUI_PlaylistChooser::retranslate_ui()
{
    ui->retranslateUi(this);
}


QString GUI_PlaylistChooser::get_name() const
{
    return "Playlists";
}


QString GUI_PlaylistChooser::get_display_name() const
{
    return Lang::get(Lang::Playlists);
}


void GUI_PlaylistChooser::all_playlists_fetched(const CustomPlaylistSkeletons& skeletons)
{
    if(!is_ui_initialized()){
        return;
    }

    QString old_text = ui->combo_playlistchooser->currentText();

    ui->combo_playlistchooser->clear();
    ui->combo_playlistchooser->addItem("", -1);

    for(const CustomPlaylistSkeleton& skeleton : skeletons)
    {
        ui->combo_playlistchooser->addItem(skeleton.name(), skeleton.id());
    }

    int cur_idx = std::max(ui->combo_playlistchooser->findText(old_text), 0);

    ui->combo_playlistchooser->setCurrentIndex(cur_idx);
}


void GUI_PlaylistChooser::playlist_selected(int idx)
{
    Q_UNUSED(idx)
    int id = m->playlist_chooser->find_playlist(ui->combo_playlistchooser->currentText());

    if(id >= 0){
        m->playlist_chooser->load_single_playlist(id);
    }
}

