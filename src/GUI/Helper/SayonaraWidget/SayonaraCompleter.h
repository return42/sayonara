#ifndef SAYONARACOMPLETER_H
#define SAYONARACOMPLETER_H

#include <QCompleter>

class QStringList;
class SayonaraCompleter :
		public QCompleter
{

public:
	SayonaraCompleter(const QStringList& lst, QObject* parent=nullptr);
	~SayonaraCompleter();

	void set_stringlist(const QStringList& lst);
};

#endif // SAYONARACOMPLETER_H
