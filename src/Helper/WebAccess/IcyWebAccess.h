#ifndef ICYWEBACCESS_H
#define ICYWEBACCESS_H

#include <QObject>
#include <QAbstractSocket>
#include "Helper/Pimpl.h"

class QUrl;
class IcyWebAccess : public QObject
{
    Q_OBJECT
    PIMPL(IcyWebAccess)

public:
    explicit IcyWebAccess(QObject *parent = 0);
    virtual ~IcyWebAccess();

    void check(const QUrl& url);

    enum class Status : quint8
    {
	WriteError=0,
	WrongAnswer,
	OtherError,
	Success
    };

    IcyWebAccess::Status status() const;


signals:
    void sig_finished();

private slots:
    void connected();
    void disconnected();
    void error_received(QAbstractSocket::SocketState socket_state);
    void data_available();

};

#endif // ICYWEBACCESS_H
