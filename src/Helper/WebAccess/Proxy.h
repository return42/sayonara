#ifndef PROXY_H
#define PROXY_H

#include "Helper/Singleton.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QObject>

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
};

#endif // PROXY_H
