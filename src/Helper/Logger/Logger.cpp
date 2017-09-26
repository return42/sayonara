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

#include "Helper/Logger/Logger.h"
#include "Helper/Logger/LogListener.h"
#include "Helper/Helper.h"
#include "Helper/Pimpl.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QPoint>

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


static QList<LogListener*> log_listeners;

struct Logger::Private
{
	Log					type;
	QString				class_name;
	std::stringstream	msg;

	Private() {}

	~Private()
	{
		QString type_str;
		std::string color;
		bool ignore=false;

		switch(type)
		{
			case Log::Info:
				color = LOG_GREEN;
				type_str = "Info";
				break;
			case Log::Warning:
				color = LOG_RED;
				type_str = "Warning";
				break;
			case Log::Error:
				color = LOG_RED;
				type_str = "Error";
				break;
			case Log::Debug:
				color = LOG_YELLOW;
				type_str = "Debug";
				break;

			case Log::Develop:
				color = LOG_YELLOW;
				type_str = "Dev";
	#ifndef DEBUG
				ignore = true;
	#endif
				break;
			default:
				color = LOG_YELLOW;
				type_str = "Debug";
				break;
		}

		if(!ignore)
		{
			std::string str(msg.str());

			std::clog << color << type_str.toStdString() << ": " << LOG_COL_END;

			if(!class_name.isEmpty()) {
				std::clog << LOG_BLUE << class_name.toStdString() << ": " << LOG_COL_END;
			}

			std::clog << str;
			std::clog << std::endl;

			for(LogListener* log_listener : log_listeners)
			{
				if(log_listener)
				{
					QString log_line = type_str + ": ";

					if(!class_name.isEmpty()){
						log_line += class_name + ": ";
					}

					log_line += QString::fromStdString(str);

					log_listener->add_log_line(log_line);
				}
			}
		}

		msg.clear();
	}
};


Logger::Logger(Log type, const QString& class_name)
{
	_m = new Logger::Private();

	_m->type = type;
	_m->class_name = class_name;
}

Logger::~Logger()
{
	delete _m;
	_m = nullptr;
}

//static
void Logger::register_log_listener(LogListener* log_listener)
{
	log_listeners << log_listener;
}

Logger& Logger::operator << (const QString& msg)
{
	(*this) << msg.toLocal8Bit().constData();
	return *this;
}

Logger& Logger::operator << (const QStringList& lst)
{
	for(const QString& str : lst){
		(*this) << str << ", ";
	}

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
	_m->msg << std::endl;

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

		_m->msg << std::hex << (unsigned int) (c & (0xff)) << " ";

		if(i % 8 == 7)
		{
			_m->msg << "\t";
			_m->msg << line_str.toLocal8Bit().constData() << std::endl;

			line_str.clear();
		}
	}

	if(!line_str.isEmpty())
	{
		for(int i=0; i<8-line_str.size(); i++)
		{
			_m->msg << "   ";
		}

		_m->msg << "\t" << line_str.toLocal8Bit().constData() << std::endl;
	}

	return *this;
}

Logger& Logger::operator << (const char* str)
{
	_m->msg << str;

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
	if(data){
		class_name = QString(data);
	}

	return Logger(type, data);
}
