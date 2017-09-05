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
