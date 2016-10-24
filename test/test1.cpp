#include <QTest>
#include <QDebug>

#include "test1.h"

void Test1::test1()
{
	qDebug() << "Running my test";
	QVERIFY( 1 == 1 );
}

QTEST_MAIN(Test1)
