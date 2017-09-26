#include "Proxy.h"
#include "Helper/Settings/Settings.h"
#include "Helper/Helper.h"
#include "Helper/Logger/Logger.h"

#include <QNetworkProxy>

Proxy::Proxy() :
	QObject(),
	SayonaraClass()
{
	REGISTER_LISTENER(Set::Proxy_Active, proxy_changed);
	REGISTER_LISTENER(Set::Proxy_Hostname, proxy_changed);
	REGISTER_LISTENER(Set::Proxy_Port, proxy_changed);
	REGISTER_LISTENER(Set::Proxy_Username, proxy_changed);
	REGISTER_LISTENER(Set::Proxy_Password, proxy_changed);
}

Proxy::~Proxy() {}

void Proxy::proxy_changed()
{
	bool active = _settings->get(Set::Proxy_Active);

	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::HttpProxy);

	if(active)
	{
		QString hostname = _settings->get(Set::Proxy_Hostname);
		int port = _settings->get(Set::Proxy_Port);
		QString username = _settings->get(Set::Proxy_Username);
		QString password = _settings->get(Set::Proxy_Password);

		QString host_string = "http://" + hostname + ":" + QString::number(port);
		sp_log(Log::Info, this) << "Using proxy: " << host_string;

		Helper::set_environment("http_proxy", host_string.toLocal8Bit().data());
		Helper::set_environment("https_proxy", host_string.toLocal8Bit().data());
		Helper::set_environment("HTTP_PROXY", host_string.toLocal8Bit().data());
		Helper::set_environment("HTTPS_PROXY", host_string.toLocal8Bit().data());

		proxy.setHostName(hostname);
		proxy.setPort(port);

		if((username.size() * password.size()) > 0){
			proxy.setUser(username);
			proxy.setPassword(password);
		}
	}

	QNetworkProxy::setApplicationProxy(proxy);
}

void Proxy::init()
{
	proxy_changed();
}
