#ifndef GENRE_H
#define GENRE_H

#include <QString>
#include "Helper/Pimpl.h"

class Genre
{
private:
	PIMPL(Genre)

public:
	Genre(const QString& name);
	Genre(const Genre& other);

	~Genre();

	static quint32 calc_id(const QString& name);
	quint32 id() const;

	QString name() const;
	void set_name(const QString& name);

	bool is_equal(const Genre& other) const;
	bool operator ==(const Genre& other) const;
	bool operator <(const Genre& other) const;
	bool operator >(const Genre& other) const;
	Genre& operator =(const Genre& other);
};


#endif // GENRE_H
