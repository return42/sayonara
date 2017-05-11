#ifndef LIBRARYINFO_H
#define LIBRARYINFO_H

#include <QString>

class LibraryInfo
{
	QString _path;
	QString _name;
	qint8 _id;

public:
	LibraryInfo();
	LibraryInfo(const QString& name, const QString& path, int id);
	LibraryInfo(const LibraryInfo& other);

	LibraryInfo& operator =(const LibraryInfo& other);

	QString name() const;
	QString path() const;
	qint8 id() const;
	bool valid() const;

	static LibraryInfo fromString(const QString& str);
	QString toString() const;

	bool operator==(const LibraryInfo& other) const;
};

#endif // LIBRARYINFO_H
