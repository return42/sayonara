/* Main.cpp */

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
 * Main.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Lucio Carreras
 */

#include "Application/Application.h"
#include "Utils/Utils.h"
#include "Utils/Parser/CommandLineParser.h"
#include "Utils/Settings/SettingRegistry.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Logger/Logger.h"
#include "GUI/Utils/GuiUtils.h"

#include <QSharedMemory>
#include <QTranslator>
#include <QFontDatabase>
#include <QtGlobal>
#include <QDir>
#include <QIcon>
#include <algorithm>
#include "Utils/Settings/SettingNotifier.h"

#ifdef Q_OS_UNIX
	#include <execinfo.h>		// backtrace
	#include <csignal>			// kill/signal
	#include <sys/types.h>		// kill
	#include <cstring>			// memcpy
	#include <unistd.h>			// STDERR_FILENO
#else

	#include <glib-2.0/glib.h>
	#undef signals
	#include <gio/gio.h>

#endif



#ifdef Q_OS_WIN
void init_gio()
{
	QString gio_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + "/gio-modules";
	QString gst_plugin_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + "/gstreamer-1.0/";

	Helper::set_environment("GST_PLUGIN_PATH", gst_plugin_path);
	Helper::set_environment("GST_PLUGIN_SYSTEM_PATH", gst_plugin_path);

	g_io_extension_point_register("gio-tls-backend");
	g_io_modules_load_all_in_directory(gio_path.toLocal8Bit().data());

	sp_log(Log::Debug, this) << "Done " << gio_path;
}
#endif

#include <stdio.h>

void segfault_handler(int sig)
{
	Q_UNUSED(sig)

#ifdef Q_OS_UNIX
	void* array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 20);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

}

bool check_for_other_instance(const CommandLineData& cmd_data, QSharedMemory* memory)
{
	if(memory->create(256, QSharedMemory::ReadWrite))
	{
		return false;
	}

	memory->attach(QSharedMemory::ReadWrite);
	if(!memory->data())
	{
		return false;
	}

	QByteArray data("Req");
	int size = 256;
	char* ptr = (char*) (memory->data());

	if(!cmd_data.files_to_play.isEmpty())
	{
		data += 'D';
		data += cmd_data.files_to_play.join('\n').toUtf8();

		size = std::min(data.size(), 255);
		if(size < data.size()){
			data.resize(256);
		}

		data[255] = '\0';
	}

	else {
		data += "E";
	}

	memory->lock();
	memcpy(ptr, data.data(), data.size());
	memory->unlock();

	Util::sleep_ms(500);

	if(memcmp(memory->data(), "Ack", 3) == 0)
	{
		sp_log(Log::Debug, "Main") << "There's probably another instance running";
		memory->detach();

		return true;
	}

	else {
		sp_log(Log::Debug, "Main") << "Other instance not responding";
	}

	return false;
}


int main(int argc, char *argv[]) 
{
	Application app(argc, argv);

	QTranslator translator;
	QString language;

#ifdef Q_OS_WIN
	init_gio();
#endif

#ifdef Q_OS_UNIX
	signal(SIGSEGV, segfault_handler);
#endif

	CommandLineData cmd_data = CommandLineParser::parse(argc, argv);
	if(cmd_data.abort){
		return 0;
	}

    QSharedMemory memory("SayonaraMemory");
	bool has_other_instance = check_for_other_instance(cmd_data, &memory)	;
	if(has_other_instance){
		return 0;
	}

	if(memory.data())
	{
		memory.lock();
		memcpy(memory.data(), "Sayonara", 8);
		memory.unlock();
	}

	/* Tell the settings manager which settings are necessary */
    if( !SettingRegistry::init() )
	{
		sp_log(Log::Error) << "Cannot initialize settings";
		return 1;
	}

	Q_INIT_RESOURCE(Icons);

#ifdef Q_OS_WIN
	Q_INIT_RESOURCE(IconsWindows);
#endif

	if(!QFile::exists( Util::sayonara_path() )) {
		QDir().mkdir( Util::sayonara_path() );
	}

	language = Settings::instance()->get(Set::Player_Language);
    translator.load(language, Util::share_path("translations"));

	if(!app.init(&translator, cmd_data.files_to_play)) {
		return 1;
	}

	app.exec();

	NotifyClassRegistry::instance()->destroy();
	return 0;
}
