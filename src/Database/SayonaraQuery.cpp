/* SayonaraQuery.cpp */

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

#include "Database/SayonaraQuery.h"
#include "Helper/Logger/Logger.h"

SayonaraQuery::SayonaraQuery(const QString& query, const QSqlDatabase& db) :
	QSqlQuery(query, db)
{
	_query_string = query;
}

SayonaraQuery::SayonaraQuery(QSqlResult * result) :
	QSqlQuery(result) {}


SayonaraQuery::SayonaraQuery(const QSqlDatabase& db) :
	QSqlQuery(db) {}

SayonaraQuery::SayonaraQuery(const QSqlQuery & other) :
	QSqlQuery(other) {}

SayonaraQuery::~SayonaraQuery() {}

bool SayonaraQuery::prepare(const QString& query)
{
	_query_string = query;

	return QSqlQuery::prepare(query);
}

void SayonaraQuery::bindValue(const QString& placeholder, const QVariant& val, QSql::ParamType param_type )
{
	QString replace_str = QString("'") + val.toString() + "'";

	_query_string.replace(placeholder + " ", replace_str + " ");
	_query_string.replace(placeholder + ",", replace_str + ",");
	_query_string.replace(placeholder + ";", replace_str + ";");
	_query_string.replace(placeholder + ")", replace_str + ")");

	QSqlQuery::bindValue(placeholder, val, param_type);
}

#ifdef DB_DEBUG
	#include <QTime>
#endif

bool SayonaraQuery::exec()
{
#ifdef DB_DEBUG
	QTime timer;
	timer.start();
#endif

	bool success = QSqlQuery::exec();

#ifdef DB_DEBUG
	sp_log(Log::Debug, this) << _query_string << ": " << timer.elapsed() << "ms";
#endif

	return success;
}

QString SayonaraQuery::get_query_string() const
{
	QString str = _query_string;
	str.prepend("\n");
	str.replace("SELECT ", "SELECT\n", Qt::CaseInsensitive);
	str.replace("FROM", "\nFROM", Qt::CaseInsensitive);
	str.replace(",", ",\n", Qt::CaseInsensitive);
	str.replace("INNER JOIN", "\nINNER JOIN", Qt::CaseInsensitive);
	str.replace("LEFT OUTER JOIN", "\nLEFT OUTER JOIN", Qt::CaseInsensitive);
	str.replace("UNION", "\nUNION", Qt::CaseInsensitive);
	str.replace("GROUP BY", "\nGROUP BY", Qt::CaseInsensitive);
	str.replace("ORDER BY", "\nORDER BY", Qt::CaseInsensitive);
	str.replace("WHERE", "\nWHERE", Qt::CaseInsensitive);
	str.replace("(", "\n(\n");
	str.replace(")", "\n)\n");

	int idx = str.indexOf("(");
	while(idx >= 0){
		int idx_close = str.indexOf(")", idx);
		int nl = str.indexOf("\n", idx);
		while(nl > 0 && nl < idx_close)
		{
			str.insert(nl + 1, '\t');
			nl = str.indexOf("\n", nl + 2);
		}

		idx = str.indexOf("(", idx_close);
	}

	while(str.contains("\n ")){
		str.replace("\n ", "\n");
	}

	while(str.contains(", ")){
		str.replace(", ", ",");
	}

	while(str.contains(" ,")){
		str.replace(" ,", ",");
	}

	while(str.contains("  ")){
		str.replace("  ", " ");
	}

	while(str.contains("\n\n")){
		str.replace("\n\n", "\n");
	}

	return str;
}

void SayonaraQuery::show_query() const
{
	sp_log(Log::Debug, this) << get_query_string();
}

void SayonaraQuery::show_error(const QString& err_msg) const
{
	sp_log(Log::Error) << "SQL ERROR: " << err_msg;
	sp_log(Log::Error) << this->lastError().text();
	sp_log(Log::Error) << this->lastError().databaseText();
	sp_log(Log::Error) << this->get_query_string();
}
