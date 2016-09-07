/* Main.cpp */

/* Copyright (C) 2011-2016 Lucio Carreras
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
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <fcntl.h>

#include "Application/application.h"

#include "Helper/Helper.h"
#include "Helper/FileHelper.h"
#include "Helper/Settings/SettingRegistry.h"

#include "GUI/Helper/GUI_Helper.h"

#include <QSharedMemory>
#include <QTranslator>
#include <QFontDatabase>
#include <algorithm>
#include <type_traits>

#include <QtGlobal>
#ifdef Q_OS_LINUX
	#include <execinfo.h>
#else

	#include <glib-2.0/glib.h>
	#undef signals
	#include <gio/gio.h>
	
#endif

int check_for_another_instance(qint64 pid) {

#ifdef Q_OS_LINUX

	QDir dir("/proc");
	dir.cd(".");
	QStringList lst = dir.entryList(QDir::Dirs);

	for(const QString& dirname : lst) {
		bool ok;
		int tmp_pid = dirname.toInt(&ok);
		if(!ok) continue;

		dir.cd(dirname);

		QFile f(dir.absolutePath() + QDir::separator() + "cmdline");
		f.open(QIODevice::ReadOnly);
		if(!f.isOpen()) {
			dir.cd("..");
			continue;
		}

		QString str = f.readLine();
		f.close();

		if(str.contains("sayonara", Qt::CaseInsensitive)) {
			if(pid != tmp_pid){
				return tmp_pid;
			}
		}

		dir.cd("..");
	}

#endif
	return 0;
}

void set_environment(const QString& key, const QString& value)
{

#ifdef Q_OS_WIN
	QString str = key + "=" + value;
	putenv(str.toLocal8Bit().constData());
	sp_log(Log::Info) << "Windows: Set environment variable " << str;
#else

	setenv(key.toLocal8Bit().constData(), value.toLocal8Bit().constData(), 1);
#endif
}


void printHelp() {
	sp_log(Log::Info) << "sayonara [options] <list>";
	sp_log(Log::Info) << "<list> can consist of either files or directories or both";
	sp_log(Log::Info) << "Options:";
	sp_log(Log::Info) << "\t--multi-instances  Run more than one instance";
	sp_log(Log::Info) << "\t--help             Print this help dialog";
	sp_log(Log::Info) << "Bye.";
}


void segfault_handler(int sig){

	Q_UNUSED(sig)

#ifdef Q_OS_LINUX

		void* array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 20);
	backtrace_symbols_fd(array, size, STDERR_FILENO);

#endif

}


QString get_current_locale_string(){
	QString lang_two = QString("sayonara_lang_") + QLocale::system().name().left(2).toLower() + ".qm";
	QString lang_four = QString("sayonara_lang_") + QLocale::system().name().toLower() + ".qm";

	if(QFile::exists(Helper::get_share_path() + lang_two)){
		return lang_two;
	}

	if(QFile::exists(Helper::get_share_path() + lang_four)){
		return lang_four;
	}

	return QString();
}

int main(int argc, char *argv[]) {

	Application app(argc, argv);

	QTranslator translator;
	QString language;
	QStringList files_to_play;

#ifdef Q_OS_WIN
	QString gio_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + QDir::separator() + "gio-modules";
	QString gst_plugin_path = Helper::File::clean_filename(QApplication::applicationDirPath()) + QDir::separator() + "gstreamer-1.0/";

	set_environment("GST_PLUGIN_PATH", gst_plugin_path);
	set_environment("GST_PLUGIN_SYSTEM_PATH", gst_plugin_path);

	g_io_extension_point_register("gio-tls-backend");
	g_io_modules_load_all_in_directory(gio_path.toLocal8Bit().data());

	sp_log(Log::Debug) << "Done " << gio_path;

#endif


#ifdef Q_OS_LINUX

	signal(SIGSEGV, segfault_handler);

#endif

	bool single_instance=true;
	/* Init files to play in argument list */
	for(int i=1; i<argc; i++) {
		QString str(argv[i]);

		if(str.compare("--help") == 0){
			printHelp();
			return 0;
		}

		if(str.compare("--multi-instances") == 0){
			single_instance = false;
		}

		else
		{
			files_to_play << Helper::File::get_absolute_filename(QString(argv[i]));
		}
	}

#ifdef Q_OS_LINUX

	int pid=0;
	if(single_instance){
		pid = check_for_another_instance(QCoreApplication::applicationPid());
	}

	if(pid > 0) {
		QSharedMemory memory("SayonaraMemory");

		if(!files_to_play.isEmpty()){

			QString filename = files_to_play[0] + "\n";
			QByteArray arr = filename.toUtf8();


			memory.attach(QSharedMemory::ReadWrite);

			if(memory.create(arr.size())){
				memory.lock();
				char* ptr = (char*) memory.data();
				int size = std::min(memory.size(), arr.size());

				memcpy(ptr,
						arr.data(),
						size);

				memory.unlock();
			}

			kill(pid, SIGUSR1);
		}

		else{
			kill(pid, SIGUSR2);
		}

		Helper::sleep_ms(500);

		if(memory.isAttached()){
			memory.detach();
		}

		sp_log(Log::Info) << "another instance is already running";

		return 0;
	}

#else
	Q_UNUSED(single_instance)

#endif

	/* Tell the settings manager which settings are necessary */
	if(! SettingRegistry::getInstance()->init() ){
		sp_log(Log::Error) << "Cannot initialize settings";
		return 1;
	}

	Q_INIT_RESOURCE(Icons);

#ifdef Q_OS_WIN
	Q_INIT_RESOURCE(IconsWindows);
#endif

	if(!QFile::exists(QDir::homePath() + QDir::separator() + ".Sayonara")) {
		QDir().mkdir(QDir::homePath() + QDir::separator() +  ".Sayonara");
	}

	language = Settings::getInstance()->get(Set::Player_Language);

	translator.load(language, Helper::get_share_path() + "translations");

	if(!app.init(&translator, files_to_play)) {
		return 1;
	}

	app.setApplicationName("Sayonara");
	app.setWindowIcon(GUI::get_icon("logo"));

	app.exec();

	return 0;
}
