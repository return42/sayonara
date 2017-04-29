#ifndef LOGLISTENER_H
#define LOGLISTENER_H

class QString;
class LogListener
{
    public:
	virtual void add_log_line(const QString& str)=0;
};


#endif // LOGLISTENER_H
