#include "IcyWebAccess.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Macros.h"

#include <QTcpSocket>
#include <QUrl>

struct IcyWebAccess::Private
{
    IcyWebAccess::Status status;
    QTcpSocket* tcp=nullptr;
    QString hostname;
    QString directory;
    QString filename;
    int port;

    Private()
    {
	status = IcyWebAccess::Status::Success;
    }

    void close_tcp()
    {
	if(tcp->isOpen()){
	    tcp->close();
	}

	tcp->deleteLater();
    }
};

IcyWebAccess::IcyWebAccess(QObject *parent) :
    QObject(parent)
{
    _m = Pimpl::make<Private>();

}

IcyWebAccess::~IcyWebAccess() {}

void IcyWebAccess::check(const QUrl& url)
{
    _m->tcp = new QTcpSocket(nullptr);
    _m->hostname = url.host(QUrl::PrettyDecoded);
    _m->port = url.port(80);
    _m->directory = url.path();
    if(!_m->directory.endsWith("/")){
	_m->directory.append("/");
    }

    _m->filename = url.fileName();

    connect(_m->tcp, &QTcpSocket::connected, this, &IcyWebAccess::connected);
    connect(_m->tcp, &QTcpSocket::disconnected, this, &IcyWebAccess::disconnected);
    connect(_m->tcp, &QTcpSocket::readyRead, this, &IcyWebAccess::data_available);

    _m->tcp->connectToHost(_m->hostname,
                           _m->port,
                           QTcpSocket::ReadWrite,
                           QAbstractSocket::AnyIPProtocol
    );
}

IcyWebAccess::Status IcyWebAccess::status() const
{
    return _m->status;
}


void IcyWebAccess::connected()
{
    QByteArray data(
                "GET " +
                    _m->directory.toLocal8Bit() +
                    _m->filename.toLocal8Bit() +
                " HTTP/1.1\r\n"
                "User-Agent: Sayonara/" + SAYONARA_VERSION + "\r\n"
                "Connection: Keep-Alive\r\n"
                "Accept-Encoding: gzip, deflate\r\n"
                "Accept-Language: en-US,*\r\n"
                "Host: " +
                  _m->hostname.toLocal8Bit() + ":" +
                  QString::number(_m->port).toLocal8Bit() + "\r\n"
                "\r\n"
    );


    qint64 bytes_written = _m->tcp->write(data.data(), data.size());
    if(bytes_written != data.size())
    {
	sp_log(Log::Warning, this) << "Could only write " << bytes_written << " bytes";
	_m->status = IcyWebAccess::Status::WriteError;
	emit sig_finished();
	_m->close_tcp();
    }
}

void IcyWebAccess::disconnected()
{
    _m->close_tcp();
}

void IcyWebAccess::error_received(QAbstractSocket::SocketState socket_state)
{
    Q_UNUSED(socket_state)

    sp_log(Log::Warning, this) << "Icy Webaccess Error: " << _m->tcp->errorString();

    _m->status = IcyWebAccess::Status::OtherError;
    _m->close_tcp();

    emit sig_finished();
}

void IcyWebAccess::data_available()
{
    QByteArray arr = _m->tcp->readAll();
    if(arr.contains("ICY 200 OK")){
	_m->status = IcyWebAccess::Status::Success;
    }

    else {
	sp_log(Log::Warning, this) << "Icy Answer Error: " << arr;
	_m->status = IcyWebAccess::Status::WrongAnswer;
    }

   _m->close_tcp();

    emit sig_finished();
}
