#include "DateFilter.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <QStringList>

static quint64 convert_timespan_to_int(Library::DateFilter::TimeSpan span, quint8 val)
{
    quint64 ret = val;
    switch(span)
    {
	case Library::DateFilter::TimeSpan::Hours:
	    return ret * 100 * 100;
	case Library::DateFilter::TimeSpan::Days:
	    return ret * 100 * 100 * 100;
	case Library::DateFilter::TimeSpan::Months:
	    return ret * 100 * 100 * 100 * 100;
	case Library::DateFilter::TimeSpan::Years:
	    return ret * 100 * 100 * 100 * 100 * 100;
	default:
	    return 0;
    }
}


struct Library::DateFilter::Private
{
    bool valid;
    QString name;
    quint64 span_from;
    quint64 span_to;
    Library::DateFilter::ChangeMode change_mode;

    Private(const QString& name_)
    {
	clear();
	name = name_;
    }

    void clear()
    {
	valid = false;
	name = QString();
	span_from = 0;
	span_to = 0;
	change_mode = ChangeMode::Created;
    }

    void check_from_to()
    {
	if(span_from > span_to){
	    quint64 span_to_tmp = span_to;
	    span_to = span_from;
	    span_from = span_to_tmp;
	}
    }
};



Library::DateFilter::DateFilter(const QString& name)
{
    _m = Pimpl::make<Library::DateFilter::Private>(name);
}

Library::DateFilter::~DateFilter(){}

Library::DateFilter::DateFilter(const DateFilter& other)
{
    _m = Pimpl::make<Library::DateFilter::Private>(other.name());
    (*_m) = *(other._m);
}

void Library::DateFilter::operator=(const DateFilter& other)
{
    (*_m) = *(other._m);
}

QString Library::DateFilter::name() const
{
    return _m->name;
}

bool Library::DateFilter::valid() const
{
    return _m->valid;
}

void Library::DateFilter::clear()
{
   _m->clear();
}

void Library::DateFilter::set_between(Library::DateFilter::TimeSpan span_from, quint8 value_from, Library::DateFilter::TimeSpan span_to, quint8 value_to, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = Helper::current_date_to_int() - convert_timespan_to_int(span_from, value_from);
    _m->span_to = Helper::current_date_to_int() - convert_timespan_to_int(span_to, value_to);
    _m->change_mode = change_mode;
    _m->valid = true;
    _m->check_from_to();
}

void Library::DateFilter::set_between(const QDateTime& from, const QDateTime& to, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = Helper::date_to_int(from);
    _m->span_to = Helper::date_to_int(to);
    _m->change_mode = change_mode;
    _m->valid = true;
    _m->check_from_to();
}

void Library::DateFilter::set_older_than(Library::DateFilter::TimeSpan span, quint8 value, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = 0;
    _m->span_to = Helper::current_date_to_int() - convert_timespan_to_int(span, value);
    _m->change_mode = change_mode;
    _m->valid = true;
}

void Library::DateFilter::set_older_than(const QDateTime& date, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = 0;
    _m->span_to = Helper::date_to_int(date);
    _m->change_mode = change_mode;
    _m->valid = true;
}


void Library::DateFilter::set_newer_than(Library::DateFilter::TimeSpan span, quint8 value, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = Helper::current_date_to_int() - convert_timespan_to_int(span, value);
    _m->span_to = Helper::current_date_to_int();
    _m->change_mode = change_mode;
    _m->valid = true;
}


void Library::DateFilter::set_newer_than(const QDateTime& date, Library::DateFilter::ChangeMode change_mode)
{
    _m->span_from = Helper::date_to_int(date);
    _m->span_to = Helper::current_date_to_int();
    _m->change_mode = change_mode;
    _m->valid = true;
}


QString Library::DateFilter::get_sql_filter(const QString& track_prefix) const
{
    if(!_m->valid){
	return QString();
    }

    QString ret;
    QStringList change_modes;
    QStringList change_modes_results;

    if(_m->change_mode == ChangeMode::Modified ||
       _m->change_mode == ChangeMode::ModifiedOrCreated)
    {
	if(track_prefix.isEmpty()){
	    change_modes << "modifydate";
	}
	else{
	    change_modes << QString(track_prefix + ".modifydate");
	}
    }

    if(_m->change_mode == ChangeMode::Created ||
       _m->change_mode == ChangeMode::ModifiedOrCreated)
    {
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
	if(_m->span_from > 0){
	    change_modes_result = change_mode + " > " + QString::number(_m->span_from);
	}

	if(_m->span_from > 0 && _m->span_to > 0){
	    change_modes_result += " AND ";
	}

	if(_m->span_to > 0){
	    change_modes_result += change_mode + " < " + QString::number(_m->span_to);
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

    sp_log(Log::Debug) << ret;
    return ret + " ";
}

QString Library::DateFilter::toString() const
{
	if(!_m->valid){
		return QString();
	}

	QStringList lst;
	lst << _m->name;
	lst << QString::number(_m->span_from);
	lst << QString::number(_m->span_to);
	lst << QString::number((int) _m->change_mode);

	return lst.join(",");
}

Library::DateFilter Library::DateFilter::fromString(const QString& str)
{
	QStringList lst = str.split(",");
	if(lst.size() != 4){
		return Library::DateFilter();
	}

	Library::DateFilter filter(lst[0]);
	filter._m->span_from = lst[1].toULongLong();
	filter._m->span_to = lst[2].toULongLong();
	filter._m->change_mode = (Library::DateFilter::ChangeMode) lst[3].toInt();

	return filter;
}


