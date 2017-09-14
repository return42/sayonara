/* SayonaraQuery.h */

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

#ifndef SAYONARAQUERY_H
#define SAYONARAQUERY_H

#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>

#include "Helper/Pimpl.h"
class DatabaseModule;

class SayonaraQuery : public QSqlQuery 
{
	PIMPL(SayonaraQuery)

private:
	explicit SayonaraQuery(const QString& query=QString(), const QSqlDatabase& db = QSqlDatabase()) = delete;
	explicit SayonaraQuery(QSqlResult * result) = delete;

public:
	explicit SayonaraQuery(const DatabaseModule* module);
	explicit SayonaraQuery(QSqlDatabase db);
	SayonaraQuery(const SayonaraQuery& other);

	virtual ~SayonaraQuery();

	bool prepare(const QString& query);
	void bindValue(const QString & placeholder, const QVariant & val, QSql::ParamType paramType = QSql::In);
	bool exec();

	QString get_query_string() const;
	void show_query() const;
	void show_error(const QString& err_msg) const;

	size_t fetched_rows();
};

#endif // SAYONARAQUERY_H
