/* LibraryItem.cpp */

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

#include "Helper/MetaData/LibraryItem.h"
#include <QList>
#include <QString>
#include "Helper/Random/RandomGenerator.h"

struct CustomField::Private
{
	QString display_name;
	QString value;
	QString id;

	Private(const QString& id, const QString& display_name, const QString& value) :
		display_name(display_name),
		value(value),
		id(id)
	{}
};


CustomField::CustomField(const QString& id, const QString& display_name, const QString& value)
{
	_m = Pimpl::make<Private>(id, display_name, value);
}

CustomField::CustomField(const CustomField &other)
{
	_m = Pimpl::make<Private>(other._m->id,
							  other._m->display_name,
							  other._m->value
	);
}

CustomField& CustomField::operator=(const CustomField& other)
{
	_m->id = other._m->id,
	_m->display_name = other._m->display_name,
	_m->value = other._m->value;

	return *this;
}

CustomField::~CustomField() {}

QString CustomField::get_id() const
{
	return _m->id;
}

QString CustomField::get_display_name() const
{
	return _m->display_name;
}

QString CustomField::get_value() const
{
	return _m->value;
}


struct LibraryItem::Private
{
	QList<CustomField>	additional_data;
	QString	cover_download_url;
	uint8_t db_id;

	Private() :
		db_id(0)
	{}
};

LibraryItem::LibraryItem()
{
	_m = Pimpl::make<Private>();
}

LibraryItem::LibraryItem(const LibraryItem& other) :
	LibraryItem()
{
	_m->additional_data = other._m->additional_data;
	_m->cover_download_url = other._m->cover_download_url;
	_m->db_id = other._m->db_id;
}

LibraryItem::LibraryItem(LibraryItem&& other)
{
	_m->additional_data = std::move(other._m->additional_data);
	_m->cover_download_url = std::move(other._m->cover_download_url);
	_m->db_id = std::move(other._m->db_id);
}

LibraryItem& LibraryItem::operator=(const LibraryItem& other)
{
	_m->additional_data = other._m->additional_data;
	_m->cover_download_url = other._m->cover_download_url;
	_m->db_id = other._m->db_id;

	return *this;
}


LibraryItem::~LibraryItem() {}

void LibraryItem::add_custom_field(const CustomField& field){
	_m->additional_data << field;
}

void LibraryItem::add_custom_field(const QString& id, const QString& display_name, const QString& value){
	_m->additional_data << CustomField(id, display_name, value);
}

const QList<CustomField>& LibraryItem::get_custom_fields() const
{
	return _m->additional_data;
}


QString LibraryItem::get_custom_field(const QString& id) const
{
	for(const CustomField& field : _m->additional_data){
		if(field.get_id().compare(id, Qt::CaseInsensitive) == 0){
			return field.get_value();
		}
	}

	return "";
}


QString LibraryItem::get_custom_field(int idx) const
{
	if(idx < 0 || idx >= _m->additional_data.size()){
		return "";
	}

	return _m->additional_data[idx].get_value();
}

QString LibraryItem::cover_download_url() const
{
	return _m->cover_download_url;
}

void LibraryItem::set_cover_download_url(const QString& url)
{
	_m->cover_download_url = url;
}

uint8_t LibraryItem::db_id() const
{
	return _m->db_id;
}

void LibraryItem::set_db_id(uint8_t id)
{
	_m->db_id = id;
}


void LibraryItem::print() const {}


