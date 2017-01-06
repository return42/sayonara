/* DateFilter.cpp */

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



#include "DateFilter.h"
#include "Helper/Helper.h"

#include <QStringList>

static QDateTime substract_span(const QDateTime& t, Library::DateFilter::TimeSpan span, int val)
{
	switch(span)
	{
	case Library::DateFilter::TimeSpan::Days:
		return t.addDays(-val);
	case Library::DateFilter::TimeSpan::Weeks:
		return t.addDays(-val * 7);
	case Library::DateFilter::TimeSpan::Months:
		return t.addMonths(-val);
	case Library::DateFilter::TimeSpan::Years:
		return t.addYears(-val);;
	default:
		return QDateTime();
	}
}


struct Library::DateFilter::Private
{
	bool valid;
	QString name;
	quint64 span_from;
	quint64 span_to;
	Library::DateFilter::TimeSpanMap filter_map;

	Library::DateFilter::Type type;
	Library::DateFilter::ChangeMode change_mode;

	Private(const QString& name_)
	{
		clear();
		name = name_;
	}

	void clear()
	{
		valid = false;
		span_from = 0;
		span_to = 0;
		change_mode = ChangeMode::Created;
		filter_map.clear();
	}

	void check_from_to()
	{
		if(span_from > span_to){
			std::swap(span_from, span_to);
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

bool Library::DateFilter::operator==(const Library::DateFilter& other) const
{
	return ((name() == other.name()) &&
			(_m->span_from == other._m->span_from) &&
			(_m->span_to == other._m->span_to) &&
			(_m->valid == other._m->valid) &&
			(_m->type == other._m->type));
}

QString Library::DateFilter::name() const
{
	return _m->name;
}

void Library::DateFilter::set_name(const QString& name)
{
	_m->name = name;
}

bool Library::DateFilter::valid() const
{
	return _m->valid;
}

void Library::DateFilter::clear()
{
	_m->clear();
}


Library::DateFilter::TimeSpanMap Library::DateFilter::time_span_map() const
{
	return _m->filter_map;
}

Library::DateFilter::Type
Library::DateFilter::type() const
{
	return _m->type;
}

void Library::DateFilter::set_between(Library::DateFilter::TimeSpan span_from, quint8 value_from, Library::DateFilter::TimeSpan span_to, quint8 value_to, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = Helper::date_to_int( substract_span(QDateTime::currentDateTime(), span_from, value_from) );
	_m->span_to = Helper::date_to_int( substract_span(QDateTime::currentDateTime(), span_to, value_to) );
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->check_from_to();
	_m->filter_map << QPair<Library::DateFilter::TimeSpan, quint8>(span_from, value_from);
	_m->filter_map << QPair<Library::DateFilter::TimeSpan, quint8>(span_to, value_to);
	_m->type = Library::DateFilter::Type::Between;
}

void Library::DateFilter::set_between(const QDateTime& from, const QDateTime& to, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = Helper::date_to_int(from);
	_m->span_to = Helper::date_to_int(to);
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->check_from_to();
	_m->type = Library::DateFilter::Type::Between;
}

void Library::DateFilter::set_older_than(Library::DateFilter::TimeSpan span, quint8 value, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = 0;
	_m->span_to = Helper::date_to_int( substract_span(QDateTime::currentDateTime(), span, value) );
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->filter_map << QPair<Library::DateFilter::TimeSpan, quint8>(span, value);
	_m->type = Library::DateFilter::Type::OlderThan;
}

void Library::DateFilter::set_older_than(const QDateTime& date, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = 0;
	_m->span_to = Helper::date_to_int(date);
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->type = Library::DateFilter::Type::OlderThan;
}


void Library::DateFilter::set_newer_than(Library::DateFilter::TimeSpan span, quint8 value, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = Helper::date_to_int( substract_span(QDateTime::currentDateTime(), span, value) );
	_m->span_to = Helper::current_date_to_int();
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->filter_map << QPair<Library::DateFilter::TimeSpan, quint8>(span, value);
	_m->type = Library::DateFilter::Type::NewerThan;
}


void Library::DateFilter::set_newer_than(const QDateTime& date, Library::DateFilter::ChangeMode change_mode)
{
	_m->clear();
	_m->span_from = Helper::date_to_int(date);
	_m->span_to = Helper::current_date_to_int();
	_m->change_mode = change_mode;
	_m->valid = true;
	_m->type = Library::DateFilter::Type::NewerThan;
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

	return ret + " ";
}

QString Library::DateFilter::toString() const
{
	QStringList lst;
	lst << name();
	lst << QString::number((int)(type()));
	for(Library::DateFilter::TimeSpanEntry entry : time_span_map())
	{
		lst << QString::number((int)(entry.first));
		lst << QString::number((int)(entry.second));
	}

	return lst.join(";");
}

Library::DateFilter Library::DateFilter::fromString(const QString& str)
{
	QStringList lst = str.split(";");
	if(lst.size() < 4){
		return Library::DateFilter();
	}

	Library::DateFilter filter(lst[0]);
	Library::DateFilter::Type type = (Library::DateFilter::Type) (lst[1].toInt());
	if(type == Library::DateFilter::Type::Between && lst.size() < 6)
	{
		return Library::DateFilter();
	}

	switch(type)
	{
		case Library::DateFilter::Type::NewerThan:
		{
			Library::DateFilter::TimeSpan span = (Library::DateFilter::TimeSpan)(lst[2].toInt());
			quint8 value = (quint8) (lst[3].toInt());

			filter.set_newer_than(span, value);
		}

		break;
		case Library::DateFilter::Type::OlderThan:
		{
			Library::DateFilter::TimeSpan span = (Library::DateFilter::TimeSpan)(lst[2].toInt());
			quint8 value = (quint8) lst[3].toInt();

			filter.set_older_than(span, value);
		}

		break;
		case Library::DateFilter::Type::Between:
		{
			Library::DateFilter::TimeSpan span_from = (Library::DateFilter::TimeSpan)(lst[2].toInt());
			quint8 value_from = (quint8)(lst[3].toInt());
			Library::DateFilter::TimeSpan span_to = (Library::DateFilter::TimeSpan)(lst[4].toInt());
			quint8 value_to = (quint8)(lst[5].toInt());

			filter.set_between(span_from, value_from, span_to, value_to);
		}

		break;
		default:
			return Library::DateFilter();
	}

	return filter;
}


