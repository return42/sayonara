#ifndef METATYPEREGISTRY_H
#define METATYPEREGISTRY_H

#include <QObject>

class MetaTypeRegistry :
		public QObject
{
	Q_OBJECT

public:
	MetaTypeRegistry(QObject* parent=nullptr);
	~MetaTypeRegistry();
};

#endif // METATYPEREGISTRY_H
