#include "Genre.h"
#include "Helper/Helper.h"
#include <QHash>

struct Genre::Private
{
	quint32 id;
	QString name;

	static quint32 calc_id(const QString& name)
	{
		return static_cast<quint32> (qHash(name.trimmed().toLower().toLocal8Bit()));
	}
};

Genre::Genre(const QString& name)
{
	_m = Pimpl::make<Private>();
	_m->name = name;
	_m->id = _m->calc_id(name);
}

Genre::~Genre() {}

quint32 Genre::calc_id(const QString& name)
{
	return Genre::Private::calc_id(name);
}

Genre::Genre(const Genre& other)
{
	_m = Pimpl::make<Private>();
	_m->name = other.name();
	_m->id = other.id();
}

Genre& Genre::operator =(const Genre& other)
{
	_m->name = (other.name());
	_m->id = (other.id());
	return *this;
}


quint32 Genre::id() const
{
	return _m->id;
}

QString Genre::name() const
{
	return Helper::cvt_str_to_first_upper(_m->name);
}

void Genre::set_name(const QString& name)
{
	_m->name = name;
	_m->id = Genre::Private::calc_id(name);
}

bool Genre::is_equal(const Genre& other) const
{
	return (_m->id == other.id());
}

bool Genre::operator ==(const Genre& other) const
{
	return is_equal(other);
}

bool Genre::operator <(const Genre& other) const
{
	return (_m->id < other.id());
}

bool Genre::operator >(const Genre& other) const
{
	return (_m->id > other.id());
}

