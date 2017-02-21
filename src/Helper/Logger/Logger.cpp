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

static std::ofstream f;

struct Logger::Private
{
	std::stringstream buffer;

	Private(const char* filename)
	{
		if(!f.is_open())
		{
			f.open(filename, std::ios_base::out);
		}
	}

	~Private()
	{
		std::string str(buffer.str());
		std::clog << str;
		std::clog << std::endl;

		if(f.is_open()){
			f << str << std::endl;
			f.flush();
			f.close();
		}
	}
};



Logger::Logger()
{
	_m = new Logger::Private(Helper::get_sayonara_path("log.out").toUtf8().data());
}

Logger::Logger(const char* msg)
{
	_m = new Logger::Private(Helper::get_sayonara_path("log.out").toUtf8().data());
	_m->buffer << msg;
}

Logger::~Logger()
{
	delete _m;
	_m = nullptr;
}

std::ostream& Logger::out()
{
	return _m->buffer;
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
	_m->buffer << std::endl;

	QString line_str;

	for(int i=0; i<arr.size(); i++){
		char c = arr[i];

		QChar qc = QChar(c);

		if(qc.isPrint()){
			line_str += qc;
		}

		else{
			line_str += ".";
		}

		_m->buffer << std::hex << (unsigned int) (c & (0xff)) << " ";

		if(i % 8 == 7){
			_m->buffer << "\t";
			_m->buffer << line_str.toLocal8Bit().constData() << std::endl;
			line_str.clear();
		}
	}

	if(!line_str.isEmpty()){
		for(int i=0; i<8-line_str.size(); i++){
			_m->buffer << "   ";
		}

		_m->buffer << "\t" << line_str.toLocal8Bit().constData() << std::endl;
	}

	return *this;
}

Logger& Logger::operator << (const char* str)
{
	_m->buffer << str;
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
	QString type_str;
	switch(type)
	{
		case Log::Info:
			break;
		case Log::Debug:
			type_str = "Debug: ";
			break;
		case Log::Warning:
			type_str = "Warning: ";
			break;
		case Log::Error:
			type_str = "Error: ";
			break;
		default:
			type_str = "Debug: ";
			break;
	}

	if(data){
		type_str += QString(data) + ":";
	}

	return Logger(type_str.toLocal8Bit().data());

}
