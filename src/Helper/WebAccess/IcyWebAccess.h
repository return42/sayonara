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
	explicit IcyWebAccess(QObject *parent = nullptr);
	~IcyWebAccess();

	enum class Status : quint8
    {
		WriteError=0,
		WrongAnswer,
		OtherError,
		NotExecuted,
		Success
    };

    IcyWebAccess::Status status() const;
	void check(const QUrl& url);


signals:
    void sig_finished();

private slots:
    void connected();
    void disconnected();
	void error_received(QAbstractSocket::SocketError socket_state);
    void data_available();

};

#endif // ICYWEBACCESS_H
