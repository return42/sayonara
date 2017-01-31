#include "AbstractFrame.h"
#include <taglib/tstring.h>
#include <QString>

struct Tagging::AbstractFrameHelper::Private
{
	QString key;
};

Tagging::AbstractFrameHelper::AbstractFrameHelper(const QString& key)
{
	_m = Pimpl::make<Private>();
	_m->key = key;
}

Tagging::AbstractFrameHelper::~AbstractFrameHelper() {}


TagLib::String Tagging::AbstractFrameHelper::cvt_string(const QString& str) const
{
	return TagLib::String(str.toUtf8().data(), TagLib::String::Type::UTF8);
}

QString Tagging::AbstractFrameHelper::cvt_string(const TagLib::String str) const
{
	return QString(str.toCString(true));
}

QString Tagging::AbstractFrameHelper::key() const
{
	return _m->key;
}

TagLib::String Tagging::AbstractFrameHelper::tag_key() const
{
	return cvt_string(_m->key);
}

