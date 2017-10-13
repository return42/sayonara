/* GlobalMessage.cpp */

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

#include "GlobalMessage.h"
#include "GlobalMessageReceiverInterface.h"
#include "Utils/Logger/Logger.h"

static GlobalMessageReceiverInterface* s_receiver=nullptr;

bool GlobalMessage::register_receiver(GlobalMessageReceiverInterface* receiver)
{
    if(s_receiver != nullptr){
        sp_log(Log::Warning) << "There's also another receiver: " << s_receiver->get_name();
		return false;
	}

    s_receiver = receiver;

	return true;
}


GlobalMessage::Answer
GlobalMessage::info(const QString& info, const QString& sender_name)
{
    if(!s_receiver) {
		return GlobalMessage::Answer::Undefined;
	}

    return s_receiver->info_received(info, sender_name);
}

GlobalMessage::Answer
GlobalMessage::warning(const QString& warning, const QString& sender_name)
{
    if(!s_receiver) {
		return GlobalMessage::Answer::Undefined;
	}

    return s_receiver->warning_received(warning, sender_name);
}

GlobalMessage::Answer
GlobalMessage::error(const QString& error, const QString& sender_name)
{
    if(!s_receiver) {
		return GlobalMessage::Answer::Undefined;
	}

    return s_receiver->error_received(error, sender_name);
}

GlobalMessage::Answer
GlobalMessage::question(const QString& question, const QString& sender_name, GlobalMessage::QuestionType type)
{
    if(!s_receiver) {
		return GlobalMessage::Answer::Undefined;
	}

    return s_receiver->question_received(question, sender_name, type);
}



