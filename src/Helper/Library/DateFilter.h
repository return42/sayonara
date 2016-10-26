#ifndef DATE_MODE_H_
#define DATE_MODE_H_
#include "Helper/Helper.h"
#include <QStringList>
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
private:

	enum class ChangeMode : char
	{
		Modified=1,
		Created=2,
		ModifiedOrCreated=3
	};

	quint64 	_span_from;
	quint64		_span_to;
	ChangeMode 	_change_mode;
	QString 	_name;
	bool		_valid;

private:
	quint64 convert_timespan_to_int(TimeSpan span, int val)
	{
		switch(span)
		{
		case TimeSpan::Hours:
			return val * 100 * 100;
		case TimeSpan::Days:
			return val * 100 * 100 * 100;
		case TimeSpan::Months:
			return val * 100 * 100 * 100 * 100;
		case TimeSpan::Years:
			return val * 100 * 100 * 100 * 100 * 100;
		}

		return val;
	}

	void check_from_to()
	{
		if(_span_from > _span_to){
			quint64 span_to = _span_to;
			_span_to = _span_from;
			_span_from = span_to;
		}
	}

public:



	DateFilter(const QString& name=QString())
	{
		clear();
		_name = name;
	}

	bool valid() const
	{
		return _valid;
	}

	void clear()
	{
		_valid = false;
		_name = QString();
		_span_from = 0;
		_span_to = 0;
		_change_mode = ChangeMode::Created;
	}

	void set_between(TimeSpan span_from, int value_from, TimeSpan span_to, int value_to, ChangeMode change_mode=ChangeMode::Created)
	{
		_span_from = Helper::current_date_to_int() - convert_timespan_to_int(span_from, value_from);
		_span_to = Helper::current_date_to_int() - convert_timespan_to_int(span_to, value_to);
		_change_mode = change_mode;
		_valid = true;
		check_from_to();
	}

	void set_between(const QDateTime& from, const QDateTime& to, ChangeMode change_mode=ChangeMode::Created)
	{
		_span_from = Helper::date_to_int(from);
		_span_to = Helper::date_to_int(to);
		_change_mode = change_mode;
		_valid = true;
		check_from_to();
	}


	void set_older_than(TimeSpan span, int value, ChangeMode change_mode=ChangeMode::Created)
	{
		_span_from = 0;
		_span_to = Helper::current_date_to_int() - convert_timespan_to_int(span, value);
		_change_mode = change_mode;
		_valid = true;
	}

	void set_older_than(const QDateTime& date, ChangeMode change_mode)
	{
		_span_from = 0;
		_span_to = Helper::date_to_int(date);
		_change_mode = change_mode;
		_valid = true;
	}


	void set_newer_than(TimeSpan span, int value, ChangeMode change_mode=ChangeMode::Created)
	{
		_span_from = Helper::current_date_to_int() - convert_timespan_to_int(span, value);
		_span_to = Helper::current_date_to_int();
		_change_mode = change_mode;
		_valid = true;
	}

	void set_newer_than(const QDateTime& date, ChangeMode change_mode=ChangeMode::Created)
	{
		_span_from = Helper::date_to_int(date);
		_span_to = Helper::current_date_to_int();
		_change_mode = change_mode;
		_valid = true;
	}

	QString get_sql_filter(const QString& track_prefix=QString()) const
	{
		if(!_valid){
			return QString();
		}

		QString ret;
		QStringList change_modes;
		QStringList change_modes_results;
		if(_change_mode == ChangeMode::Modified || _change_mode == ChangeMode::ModifiedOrCreated){
			if(track_prefix.isEmpty()){
				change_modes << "modifydate";
			}
			else{
				change_modes << QString(track_prefix + ".modifydate");
			}
		}

		if(_change_mode == ChangeMode::Created || _change_mode == ChangeMode::ModifiedOrCreated){
			if(track_prefix.isEmpty()){
				change_modes << "createdate";
			}
			else{
				change_modes << QString(track_prefix + ".createdate");
			}
		}

		for(const QString& change_mode : change_modes)
		{
			QString change_modes_result;
			if(_span_from > 0){
				change_modes_result = change_mode + " > " + QString::number(_span_from);
			}

			if(_span_from > 0 && _span_to > 0){
				change_modes_result += " AND ";
			}

			if(_span_to > 0){
				change_modes_result += change_mode + " < " + QString::number(_span_to);
			}

			change_modes_results << change_modes_result;
		}

		if(change_modes_results.size() == 1){
			ret = change_modes_results[0];
		}

		else if(change_modes_results.size() == 2){
			ret = "(" + change_modes_results[0] + ")";
			ret += " OR ";
			ret += "(" + change_modes_results[1] + ")";
		}

		return ret + " ";
	}
};
}

#endif
