#include "LibraryInfo.h"
#include "Helper/FileHelper.h"

#include <QStringList>

LibraryInfo::LibraryInfo()
{
	_id = -1;
}

LibraryInfo::LibraryInfo(const QString& name, const QString& path, int id)
{
	_name = name;
	_path = Helper::File::clean_filename(path);
	_id = id;
}


QString LibraryInfo::name() const
{
	return _name;
}

QString LibraryInfo::path() const
{
	return _path;
}

qint8 LibraryInfo::id() const
{
	return _id;
}

bool LibraryInfo::valid() const
{
	return (!_name.isEmpty()) && (!_path.isEmpty());
}

LibraryInfo LibraryInfo::fromString(const QString& str)
{
	QStringList lst = str.split("::");
	if(lst.size() != 3){
		return LibraryInfo();
	}

	bool ok;
	QString name = lst[0];
	QString path = lst[1];
	int id = lst[2].toInt(&ok);
	if(!ok){
		return LibraryInfo();
	}

	return LibraryInfo(name, path, id);
}

QString LibraryInfo::toString() const
{
	QStringList lst;
	lst << _name;
	lst << _path;
	lst << QString::number(_id);

	return lst.join("::");
}


bool LibraryInfo::operator==(const QString& path) const
{
	QString cleaned_path = Helper::File::clean_filename(path);
	return (cleaned_path.compare(_path, Qt::CaseInsensitive) == 0);
}

bool LibraryInfo::operator==(const LibraryInfo& li) const
{
	QString cleaned_path = Helper::File::clean_filename(li.path());
	return (cleaned_path.compare(_path, Qt::CaseInsensitive) == 0);
}
