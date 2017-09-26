#ifndef PROXY_H
#define PROXY_H

#include "Helper/Singleton.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QObject>

class QString;
class Proxy :
		public QObject,
		public SayonaraClass
{
	Q_OBJECT
	SINGLETON(Proxy)

	private slots:
		void proxy_changed();

	public:
		void init();

		QString hostname() const;
		int port() const;
		QString username() const;
		QString password() const;
		bool active() const;
		bool has_username() const;

		QString full_url() const;
};

#endif // PROXY_H
