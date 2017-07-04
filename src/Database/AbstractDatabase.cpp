/* AbstractDatabase.cpp */

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

#include "Database/AbstractDatabase.h"
#include "Database/DatabaseModule.h"
#include "Database/SayonaraQuery.h"

#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <QFile>
#include <QDir>

static QMap<uint8_t, QSqlDatabase> _databases;

struct AbstractDatabase::Private
{
	QString db_name;
	QString db_dir;
	QString db_path;
	uint8_t db_id;

	bool initialized;

	Private(uint8_t db_id, const QString& db_dir, const QString& db_name)
	{
		this->db_id = db_id;
		this->db_dir = db_dir;
		this->db_name = db_name;
		this->db_path = Helper::sayonara_path(db_name);
	}
};

AbstractDatabase::AbstractDatabase(uint8_t db_id, const QString& db_dir, const QString& db_name, QObject *parent) : QObject(parent)
{
	_m = Pimpl::make<Private>(db_id, db_dir, db_name);

	if(!exists()){
		sp_log(Log::Info, this) << "Database not existent. Creating database...";
		create_db();
	}

	_m->initialized = open_db();

	if(!_m->initialized) {
		sp_log(Log::Error, this) << "Could not open database";
	}
}

AbstractDatabase::~AbstractDatabase()
{
	close_db();

	for(const QString& connection_name : QSqlDatabase::connectionNames()){
		QSqlDatabase::removeDatabase(connection_name);
	}
}


bool AbstractDatabase::is_initialized()
{
	return _m->initialized;
}

bool AbstractDatabase::exists()
{
	return QFile::exists(_m->db_path);
}


bool AbstractDatabase::open_db()
{
	if(_databases.contains(_m->db_id))
	{
		return true;
	}

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", _m->db_path);
	db.setDatabaseName(_m->db_path);

	bool success = db.open();
	if (!success) {
		sp_log(Log::Error, this) << "DatabaseConnector database cannot be opened!";
		QSqlError er = db.lastError();
		sp_log(Log::Error) << er.driverText();
		sp_log(Log::Error) << er.databaseText();
	}
	else{
		sp_log(Log::Info, this) << "Opened Database " << _m->db_name;
	}

	_databases.insert(_m->db_id, db);

	return success;
}

void AbstractDatabase::close_db()
{
	if(!_databases.contains(_m->db_id)){
		return;
	}

	sp_log(Log::Info) << "close database " << _m->db_name << "...";

	if(db().isOpen()){
		db().close();
	}

	_databases.remove(_m->db_id);
}


bool AbstractDatabase::create_db()
{
	bool success;
	QDir dir = QDir::homePath();

	QString sayonara_path = Helper::sayonara_path();
	if(!QFile::exists(sayonara_path)) {
		success = dir.mkdir(".Sayonara");
		if(!success) {
			sp_log(Log::Error) << "Could not create .Sayonara dir";
			return false;
		}

		else{
			sp_log(Log::Info) << "Successfully created .Sayonara dir";
		}
	}

	success = dir.cd(sayonara_path);

	//if ret is still not true we are not able to create the directory
	if(!success) {
		sp_log(Log::Error) << "Could not change to .Sayonara dir";
		return false;
	}

	QString source_db_file = Helper::share_path(_m->db_dir + "/" + _m->db_name);

	success = QFile::exists(_m->db_path);

	if(success) {
		return true;
	}

	if (!success) {
		sp_log(Log::Info) << "Database " << _m->db_path << " not existent yet";
		sp_log(Log::Info) << "Copy " <<  source_db_file << " to " << _m->db_path;

		if (QFile::copy(source_db_file, _m->db_path)) {
			sp_log(Log::Info) << "DB file has been copied to " <<   _m->db_path;
			success = true;
		}

		else {
			sp_log(Log::Error) << "Fatal Error: could not copy DB file to " << _m->db_path;
			success = false;
		}
	}

	return success;
}


void AbstractDatabase::transaction()
{
	db().transaction();
}

void AbstractDatabase::commit()
{
	db().commit();
}

void AbstractDatabase::rollback()
{
	db().rollback();
}


bool AbstractDatabase::check_and_drop_table(const QString& tablename)
{
	SayonaraQuery q(db());
	QString querytext = "DROP TABLE " +  tablename + ";";
	q.prepare(querytext);

	if(!q.exec()){
		q.show_error(QString("Cannot drop table ") + tablename);
		return false;
	}

	return true;
}


bool AbstractDatabase::check_and_insert_column(const QString& tablename, const QString& column, const QString& sqltype, const QString& default_value)
{
	SayonaraQuery q(db());
	QString querytext = "SELECT " + column + " FROM " + tablename + ";";
	q.prepare(querytext);

	if(!q.exec()) {
		SayonaraQuery q2 (db());
		querytext = "ALTER TABLE " + tablename + " ADD COLUMN " + column + " " + sqltype;
		if(!default_value.isEmpty()){
			querytext += " DEFAULT " + default_value;
		}

		querytext += ";";

		q2.prepare(querytext);

		if(!q2.exec()){;
			q.show_error(QString("Cannot insert column ") + column + " into " + tablename);
			return false;
		}

		return true;
	}

	return true;
}

bool AbstractDatabase::check_and_create_table(const QString& tablename, const QString& sql_create_str)
{
	SayonaraQuery q(db());
	QString querytext = "SELECT * FROM " + tablename + ";";
	q.prepare(querytext);

	if(!q.exec())
	{
		SayonaraQuery q2 (db());
		q2.prepare(sql_create_str);

		if(!q2.exec()){
			q.show_error(QString("Cannot create table ") + tablename);
			return false;
		}
	}

	return true;
}

QSqlDatabase& AbstractDatabase::db() const
{
	return _databases[ db_id() ];
}

uint8_t AbstractDatabase::db_id() const
{
	return _m->db_id;
}

