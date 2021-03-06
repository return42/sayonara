/* EngineHandler.h */

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

#ifndef ENGINEHANDLER_H_
#define ENGINEHANDLER_H_

#include "AbstractEngine.h"
#include "Components/PlayManager/PlayState.h"
#include "Utils/Singleton.h"
#include "Utils/Pimpl.h"

#include <QList>

#define EngineHandler_change_track_md static_cast<void (EngineHandler::*) (const MetaData& md)>(&EngineHandler::change_track)

class RawSoundReceiverInterface;

namespace Engine
{
    class Playback;

    /**
     * @brief The EngineHandler class
     * @ingroup Engine
     */
    class Handler :
            public Base
    {
        Q_OBJECT
        SINGLETON_QOBJECT(Handler)
        PIMPL(Handler)

    public:
        bool init() override;

        Engine::Playback* get_playback_engine();

        void register_raw_sound_receiver(RawSoundReceiverInterface* receiver);
        void unregister_raw_sound_receiver(RawSoundReceiverInterface* receiver);

        void start_convert();
        void end_convert();

        void set_equalizer(int band, int value);

        bool change_track(const MetaData&) override;
        bool change_track_by_filename(const QString&) override;


    private slots:
        void jump_abs_ms(uint64_t ms) override;
        void jump_rel_ms(uint64_t ms) override;
        void jump_rel(double where) override;

        void sl_md_changed(const MetaData&);
        void sl_dur_changed(const MetaData&);
        void sl_pos_changed_ms(uint64_t ms);
        void sl_pos_changed_s(uint32_t s);

        void sl_track_finished();

        void sl_track_ready_changed();
        void sl_buffer_state_changed(int progress);

        void sl_error(const QString& error_msg);

        void sr_record_button_pressed(bool);
        void playstate_changed(PlayState state);

        void new_data(const uchar* data, uint64_t n_bytes);

        void play() override;
        void stop() override;
        void pause() override;

    private:
        Base* get_engine(Name name);
        void switch_engine(Name name);

        bool configure_connections(Base* old_engine, Base* new_engine);

        // Engine interface
    protected:
        bool change_uri(char* uri) override;
    };
}
#endif


