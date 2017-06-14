/* Logger.h */

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

#ifndef LOGGER_H
#define LOGGER_H

#include <iosfwd>
#include <typeinfo>
#include <type_traits>

/**
 * @brief The Log enum
 * @ingroup Helper
 */
class QString;
class QStringList;
class QByteArray;
class QPoint;
class QChar;
class QObject;
class QRect;
class QSize;
class LogListener;
enum class Log : unsigned char
{
	Warning,
	Error,
	Info,
	Debug,
	Develop
};

/**
 * @brief The Logger class
 * @ingroup Helper
 */
class Logger
{




private:
	struct Private;
	Private* _m=nullptr;

	std::ostream& out();

public:
	explicit Logger(bool ignore=false);
	explicit Logger(const char* msg, bool ignore=false);

	~Logger();

	static void register_log_listener(LogListener* log_listener);

	Logger& operator << (const QString& msg);
	Logger& operator << (const QChar& c);
	Logger& operator << (const QStringList& lst);
	Logger& operator << (const QByteArray& arr);
	Logger& operator << (const QPoint& point);
	Logger& operator << (const QRect& rect);
	Logger& operator << (const QSize& size);
	Logger& operator << (const char* str);

	template <typename T>
	Logger& operator << (const T& msg){
		out() << msg;
		return *this;
	}

	template<typename T, template <typename ELEM> class CONT>
	Logger& operator << (const CONT<T> list){
		for(const T& item : list){
			(*this) << item << ", ";
		}

		return *this;
	}
};

Logger sp_log(Log type);
Logger sp_log(Log type, const char* data);

template<typename T>
typename std::enable_if< std::is_class<T>::value, Logger>::type
sp_log(Log type, const T*)
{
	return sp_log(type, typeid(T).name());
}

#endif // LOGGER_H
