#ifndef DATE_MODE_H_
#define DATE_MODE_H_

#include "Helper/Pimpl.h"
#include <QString>
#include <QDateTime>

namespace Library
{
    class DateFilter
    {

    public:
	enum class TimeSpan : char
	{
	    Hours,
	    Days,
	    Months,
	    Years
	};

	enum class ChangeMode : char
	{
	    Modified=1,
	    Created=2,
	    ModifiedOrCreated=3
	};


    private:
	PIMPL(Library::DateFilter)

    public:

	DateFilter(const QString& name=QString());
	DateFilter(const DateFilter& other);
	void operator=(const DateFilter& other);

	~DateFilter();

	QString name() const;
	bool valid() const;
	void clear();

	void set_between(TimeSpan span_from, quint8 value_from, TimeSpan span_to, quint8 value_to, ChangeMode change_mode=ChangeMode::Created);
	void set_between(const QDateTime& from, const QDateTime& to, ChangeMode change_mode=ChangeMode::Created);

	void set_older_than(TimeSpan span, quint8 value, ChangeMode change_mode=ChangeMode::Created);
	void set_older_than(const QDateTime& date, ChangeMode change_mode);

	void set_newer_than(TimeSpan span, quint8 value, ChangeMode change_mode=ChangeMode::Created);
	void set_newer_than(const QDateTime& date, ChangeMode change_mode=ChangeMode::Created);

	QString get_sql_filter(const QString& track_prefix=QString()) const;

	QString toString() const;
	static Library::DateFilter fromString(const QString& str);
    };
}

#endif
