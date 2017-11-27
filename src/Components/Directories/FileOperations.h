#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include "Utils/Pimpl.h"

#include <QThread>

class QString;
class QStringList;

class DirectoryCopyThread : public QThread
{
	Q_OBJECT
	PIMPL(DirectoryCopyThread)

public:
	DirectoryCopyThread(QObject* parent, LibraryId target_library_id, const QStringList& source_dirs, const QString& target_dir);
	~DirectoryCopyThread();

	LibraryId target_library() const;

protected:
	void run() override;
};

class FileCopyThread : public QThread
{
	Q_OBJECT
	PIMPL(FileCopyThread)

public:
	FileCopyThread(QObject* parent, LibraryId target_library_id, const QStringList& source_files, const QString& target_dir);
	~FileCopyThread();

	LibraryId target_library() const;

protected:
	void run() override;
};


class FileOperations : public QObject
{
	Q_OBJECT

signals:
	void sig_copy_finished();
	void sig_copy_started();

public:
	explicit FileOperations(QObject *parent = 0);
	~FileOperations();

	bool move_dirs(const QStringList& source_dirs, const QString& target_dir);
	bool copy_dirs(const QStringList& source_dirs, const QString& target_dir);
	bool rename_dir(const QString& source_dir, const QString& target_dir);

	bool move_files(const QStringList& files, const QString& target_dir);
	bool copy_files(const QStringList& files, const QString& target_dir);

	bool rename_file(const QString& old_name, const QString& new_name);

private slots:
	void copy_dir_thread_finished();
	void copy_file_thread_finished();

};

#endif // FILEOPERATIONS_H
