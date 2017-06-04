#include "AbstractTaggingTest.h"
#include "Helper/FileHelper.h"

#include <QString>
#include <QByteArray>
#include <QString>
#include <QDir>
#include <QTest>

void AbstractTaggingTest::id3_test()
{
	_filename = QDir::tempPath() + "/sayonara-test.mp3";
	_resource_filename = ":/test/mp3test.mp3";

	run();
}

void AbstractTaggingTest::xiph_test()
{
	_filename = QDir::tempPath() + "/sayonara-test.ogg";
	_resource_filename = ":/test/oggtest.ogg";

	run();
}

void AbstractTaggingTest::init()
{
	QByteArray content;
	Helper::File::read_file_into_byte_arr(_resource_filename, content);
	Helper::File::write_file(content, _filename);
}

void AbstractTaggingTest::cleanup()
{
	Helper::File::delete_files({_filename});
}

void AbstractTaggingTest::run()
{
	init();
	run_test(_filename);
	cleanup();
}

