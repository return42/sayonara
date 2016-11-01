#include "Filter.h"
#include "DateFilter.h"

struct Library::Filter::Private
{
    Library::DateFilter	    date_filter;
    QString		    filtertext;
    Library::Filter::Mode   mode;

};

Library::Filter::Filter()
{
    _m = Pimpl::make<Library::Filter::Private>();
    clear();
}


Library::Filter::Filter(const Library::Filter& other)
{
    _m = Pimpl::make<Library::Filter::Private>();
    *(_m) = *(other._m);

}

void Library::Filter::operator=(const Filter& other)
{
    *(_m) = *(other._m);
}

Library::Filter::~Filter(){}


Library::DateFilter Library::Filter::date_filter() const
{
    return _m->date_filter;
}

void Library::Filter::set_date_filter(const Library::DateFilter& filter)
{
    _m->date_filter = filter;
}

QString Library::Filter::filtertext() const
{
    return _m->filtertext;
}

void Library::Filter::set_filtertext(const QString& str)
{
    _m->filtertext = str;
}

Library::Filter::Mode Library::Filter::mode() const
{
    return _m->mode;
}

void Library::Filter::set_mode(Library::Filter::Mode mode)
{
    _m->mode = mode;
}

bool Library::Filter::cleared() const
{
    return !(_m->date_filter.valid() ||
	    _m->filtertext.size() > 0);
}

void Library::Filter::clear()
{
    _m->date_filter = DateFilter("");
    _m->filtertext = QString();
    _m->mode = Mode::Fulltext;
}


