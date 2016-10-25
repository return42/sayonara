#include <QTest>
#include <QDebug>

#include "Helper/FileHelper.h"
#include "test1.h"
using namespace Helper::File;
void Test1::test1()
{
	QString some_path = "/path/./to/my/home/folder/bla.txt";
	QString cleaned = clean_filename(some_path);
	QString extension = calc_file_extension(some_path);
	QString parent = get_parent_directory(some_path);
	QString filename = get_filename_of_path(some_path);

	QString d, f;
	split_filename(some_path, d, f);

	qDebug() << "Cleaned..." << cleaned;
	QVERIFY( cleaned == "/path/to/my/home/folder/bla.txt" );

	qDebug() << "Extension...";
	QVERIFY( extension == "txt" );

	qDebug() << "Parent...";
	QVERIFY( parent == "/path/to/my/home/folder/" );

	qDebug() << "filename...";
	QVERIFY( filename == "bla.txt" );
}

QTEST_MAIN(Test1)
