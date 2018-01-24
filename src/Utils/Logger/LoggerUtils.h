#ifndef LOGGERUTILS
#define LOGGERUTILS

#endif // LOGGERUTILS

#include <QString>

enum class Log : unsigned char;

struct LogEntry
{
	Log type;
	QString class_name;
	QString message;
};
