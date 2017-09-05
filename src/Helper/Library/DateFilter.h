/* DateFilter.h */

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

#ifndef DATE_MODE_H_
#define DATE_MODE_H_

#include "Helper/Pimpl.h"

#include <QString>
#include <QList>
#include <QPair>


class QDateTime;
namespace Library
{
	/**
	 * @brief The DateFilter class
	 * @ingroup LibraryHelper
	 */
    class DateFilter
	{
    public:


		enum class TimeSpan : char
		{
			Days=1,
			Weeks,
			Months,
			Years
		};

		typedef QPair<Library::DateFilter::TimeSpan, uint8_t> TimeSpanEntry;
		typedef QList< TimeSpanEntry > TimeSpanMap;

		enum class ChangeMode : char
		{
			Modified=1,
			Created=2,
			ModifiedOrCreated=3
		};

		enum class Type : char
		{
			NewerThan=1,
			OlderThan,
			Between
		};

    private:
		PIMPL(Library::DateFilter)

    public:

		explicit DateFilter(const QString& name=QString());
		DateFilter(const DateFilter& other);
		void operator=(const DateFilter& other);
		bool operator==(const DateFilter& other) const;

		~DateFilter();

		QString name() const;
		void set_name(const QString& name);
		bool valid() const;
		void clear();

		void set_between(TimeSpan span_from, uint8_t value_from, TimeSpan span_to, uint8_t value_to, ChangeMode change_mode=ChangeMode::Created);
		void set_between(const QDateTime& from, const QDateTime& to, ChangeMode change_mode=ChangeMode::Created);

		void set_older_than(TimeSpan span, uint8_t value, ChangeMode change_mode=ChangeMode::Created);
		void set_older_than(const QDateTime& date, ChangeMode change_mode);

		void set_newer_than(TimeSpan span, uint8_t value, ChangeMode change_mode=ChangeMode::Created);
		void set_newer_than(const QDateTime& date, ChangeMode change_mode=ChangeMode::Created);

		TimeSpanMap time_span_map() const;
		Library::DateFilter::Type type() const;

		QString get_sql_filter(const QString& track_prefix=QString()) const;

		QString toString() const;
		static Library::DateFilter fromString(const QString& str);
	};
}

#endif
