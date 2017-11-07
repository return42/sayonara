#ifndef COVERCHANGENOTIFIER_H
#define COVERCHANGENOTIFIER_H

#include <QObject>
#include "Utils/Singleton.h"

namespace Cover
{
	class ChangeNotfier : public QObject
	{
		Q_OBJECT
		SINGLETON(ChangeNotfier)

	public:
		void shout();

	signals:
		void sig_covers_changed();
	};
}

#endif // COVERCHANGENOTIFIER_H
