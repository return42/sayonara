#include <QTest>
#include <QObject>
#include "Helper/FileHelper.h"

class FileHelperTest : public QObject
{
	Q_OBJECT

private slots:
	void test();
	void create_and_delete();
};


using namespace Helper::File;
void FileHelperTest::test()
{
	QString some_path = "/path/./to//my/home/folder/bla.txt";
	QString cleaned = clean_filename(some_path);
	QString extension = calc_file_extension(some_path);
	QString parent = get_parent_directory(some_path);
	QString filename = get_filename_of_path(some_path);

	QString d, f;
	split_filename(some_path, d, f);

	QVERIFY( cleaned == "/path/to/my/home/folder/bla.txt" );
	QVERIFY( extension == "txt" );
	QVERIFY( parent == "/path/to/my/home/folder" );
	QVERIFY( filename == "bla.txt" );
	QVERIFY(d == parent);
	QVERIFY(f == filename);
}

void FileHelperTest::create_and_delete()
{
	bool success;
	QString new_dir, new_file;
	QStringList to_be_deleted;

	/** Absolute **/
	new_dir = "/tmp/sayonara/some/absolute/filepath";
	new_file = new_dir + "/file.out";
	to_be_deleted << "/tmp/sayonara";

	QVERIFY(is_absolute(new_file));

	success = create_directories(new_dir);
	QVERIFY(success);
	QVERIFY(check_file(new_dir));

	write_file("Some data", new_file);
	QVERIFY(check_file(new_file));
	delete_files(to_be_deleted);
	QVERIFY( !check_file("/tmp/sayonara") );

	/** Relative **/
	to_be_deleted.clear();
	new_dir = "./tmp/sayonara/some/relative/filepath";
	new_file = new_dir + "/file.out";
	to_be_deleted << "./tmp";

	QVERIFY(!is_absolute(new_file));

	success = create_directories(new_dir);
	QVERIFY(success);
	QVERIFY(check_file(new_dir));

	write_file("Some data", new_file);
	QVERIFY(check_file(new_file));
	delete_files(to_be_deleted);
	QVERIFY( !check_file("./tmp") );
}

QTEST_MAIN(FileHelperTest)

#include "FileHelperTest.moc"
