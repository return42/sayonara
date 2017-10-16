/* ChangeOperations.h */

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



#ifndef CHANGEOPERATIONS_H
#define CHANGEOPERATIONS_H

#include <QString>
class LibraryManager;

class ChangeOperation
{
protected:
	LibraryManager* _library_manager=nullptr;

public:
	ChangeOperation();
	virtual ~ChangeOperation();
	virtual void exec()=0;
};

class MoveOperation :
	public ChangeOperation
{
private:
	int _from, _to;

public:
	MoveOperation(int from, int to);
	void exec() override;
};


class RenameOperation :
	public ChangeOperation
{
private:
	int8_t _id;
	QString _new_name;

public:
	RenameOperation(int8_t id, const QString& new_name);
	void exec() override;
};

class RemoveOperation :
		public ChangeOperation
{
private:
	int8_t _id;

public:
	RemoveOperation(int8_t id);

	void exec() override;
};

class AddOperation :
		public ChangeOperation
{
private:
	QString _name, _path;

public:
	AddOperation(const QString& name, const QString& path);

	void exec() override;
};

class ChangePathOperation :
		public ChangeOperation
{
private:
	int8_t _id;
	QString _new_path;

public:
	ChangePathOperation(int8_t id, const QString& new_path);

	void exec() override;
};




#endif // CHANGEOPERATIONS_H
