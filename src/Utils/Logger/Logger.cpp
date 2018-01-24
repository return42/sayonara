/* Logger.cpp */

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

#include "Utils/Logger/Logger.h"
#include "Utils/Logger/LogListener.h"
#include "Utils/Utils.h"
#include "Utils/Pimpl.h"
#include "Utils/Settings/Settings.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPoint>
#include <QDateTime>

#include <ostream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef Q_OS_UNIX
	#define LOG_RED "\x1B[31m"
	#define LOG_GREEN "\x1B[32m"
	#define LOG_BLUE "\x1B[34m"
	#define LOG_YELLOW "\x1B[33m"
	#define LOG_COL_END "\x1B[0m"
#else
	#define LOG_RED ""
	#define LOG_GREEN ""
	#define LOG_BLUE ""
	#define LOG_YELLOW ""
	#define LOG_COL_END ""
#endif

#ifdef HAVE_CXX_ABI
	#include "cxxabi.h"
#endif



static QList<LogListener*>	log_listeners;
static QList<LogEntry>		log_buffer;

struct Logger::Private
{
	QString				class_name;
	std::stringstream	msg;
	Log					type;

	Private() {}

	~Private()
	{
		QString type_str;
		std::string color;
		QString html_color;
		bool ignore=false;

		Settings* s = Settings::instance();
		int logger_level = s->get(Set::Logger_Level);

		switch(type)
		{
			case Log::Info:
				color = LOG_GREEN;
				html_color = "#00AA00";
				type_str = "Info";
				break;
			case Log::Warning:
				color = LOG_RED;
				html_color = "#EE0000";
				type_str = "Warning";
				break;
			case Log::Error:
				color = LOG_RED;
				html_color = "#EE0000";
				type_str = "Error";
				break;
			case Log::Debug:
				color = LOG_YELLOW;
				html_color = "#7A7A00";
				type_str = "Debug";
				if(logger_level < 1)
				{
					ignore = true;
				}
				break;
			case Log::Develop:
				color = LOG_YELLOW;
				html_color = "#7A7A00";
				type_str = "Dev";
				if(logger_level < 2){
					ignore = true;
				}

				break;
			case Log::Crazy:
				color = LOG_YELLOW;
				html_color = "#7A7A00";
				type_str = "CrazyLog";
				if(logger_level < 3){
					ignore = true;
				}

				break;
			default:
				color = LOG_YELLOW;
				type_str = "Debug";
				break;
		}

		if(!ignore)
		{
			QString date_time = QDateTime::currentDateTime().toString("hh:mm:ss");

			std::string str(msg.str());

			std::clog
					<< "[" << date_time.toStdString() << "] "
					<< color
					<< type_str.toStdString() << ": "
					<< LOG_COL_END;

			if(!class_name.isEmpty()) {
				std::clog << LOG_BLUE << class_name.toStdString() << ": " << LOG_COL_END;
			}

			std::clog << str;
			std::clog << std::endl;

			LogEntry le;
				le.class_name = class_name;
				le.dt = QDateTime::currentDateTime();
				le.message = QString::fromStdString(str);
				le.type = type;

			log_buffer << le;

			for(LogListener* log_listener : log_listeners)
			{
				if(log_listener)
				{
					log_listener->add_log_line(le);
				}
			}
		}

		msg.clear();
	}
};


Logger::Logger(Log type, const QString& class_name)
{
	m = new Logger::Private();

	m->type = type;
	m->class_name = class_name;
}

Logger::~Logger()
{
	delete m;
	m = nullptr;
}

//static
void Logger::register_log_listener(LogListener* log_listener)
{
	for(const LogEntry& le : log_buffer)
	{
		log_listener->add_log_line(le);
	}

	log_listeners << log_listener;
}

Logger& Logger::operator << (const QString& msg)
{
	(*this) << msg.toLocal8Bit().constData();
	return *this;
}

Logger& Logger::operator << (const QStringList& lst)
{
	(*this) << lst.join(",");

	return *this;
}

Logger& Logger::operator << (const QChar& c)
{
	(*this) << c.toLatin1();

	return *this;
}

Logger& Logger::operator << (const QPoint& point)
{
	(*this) << "Point(" << point.x() << "," << point.y() << ")";
	return *this;
}

Logger& Logger::operator << (const QByteArray& arr)
{
	m->msg << std::endl;

	QString line_str;

	for(int i=0; i<arr.size(); i++)
	{
		char c = arr[i];

		QChar qc = QChar(c);

		if(qc.isPrint()){
			line_str += qc;
		}

		else{
			line_str += ".";
		}

		m->msg << std::hex << (unsigned int) (c & (0xff)) << " ";

		if(i % 8 == 7)
		{
			m->msg << "\t";
			m->msg << line_str.toLocal8Bit().constData() << std::endl;

			line_str.clear();
		}
	}

	if(!line_str.isEmpty())
	{
		for(int i=0; i<8-line_str.size(); i++)
		{
			m->msg << "   ";
		}

		m->msg << "\t" << line_str.toLocal8Bit().constData() << std::endl;
	}

	return *this;
}

Logger& Logger::operator << (const char* str)
{
	m->msg << str;

	return *this;
}

Logger& Logger::operator << (const std::string& str)
{
	(*this) << str.c_str();
	return *this;
}


/*************************
 * Static Log functions
 * ***********************/
Logger sp_log(Log type)
{
	return sp_log(type, nullptr);
}


Logger sp_log(Log type, const char* data)
{
	QString class_name;
	if(data)
	{
#ifdef HAVE_CXX_ABI
		int status;
		class_name = QString(abi::__cxa_demangle(data, 0, 0, &status));
#else
		class_name = QString(data);
#endif
	}

	return Logger(type, class_name);
}
