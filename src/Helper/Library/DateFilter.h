#ifndef DATE_MODE_H_
#define DATE_MODE_H_

#include "Helper/Pimpl.h"

#include <QString>
#include <QDateTime>
#include <QList>
#include <QPair>

namespace Library
{
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

	typedef QPair<Library::DateFilter::TimeSpan, quint8> TimeSpanEntry;
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

	DateFilter(const QString& name=QString());
	DateFilter(const DateFilter& other);
	void operator=(const DateFilter& other);
	bool operator==(const DateFilter& other) const;

	~DateFilter();

	QString name() const;
	void set_name(const QString& name);
	bool valid() const;
	void clear();

	void set_between(TimeSpan span_from, quint8 value_from, TimeSpan span_to, quint8 value_to, ChangeMode change_mode=ChangeMode::Created);
	void set_between(const QDateTime& from, const QDateTime& to, ChangeMode change_mode=ChangeMode::Created);

	void set_older_than(TimeSpan span, quint8 value, ChangeMode change_mode=ChangeMode::Created);
	void set_older_than(const QDateTime& date, ChangeMode change_mode);

	void set_newer_than(TimeSpan span, quint8 value, ChangeMode change_mode=ChangeMode::Created);
	void set_newer_than(const QDateTime& date, ChangeMode change_mode=ChangeMode::Created);

	TimeSpanMap time_span_map() const;
	Library::DateFilter::Type type() const;

	QString get_sql_filter(const QString& track_prefix=QString()) const;


	QString toString() const;
	static Library::DateFilter fromString(const QString& str);
	};

}

#endif
