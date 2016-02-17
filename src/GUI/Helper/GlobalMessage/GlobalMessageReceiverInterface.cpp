#include "GlobalMessageReceiverInterface.h"

GlobalMessageReceiverInterface::GlobalMessageReceiverInterface(const QString &name)
{
		_name = name;
}

QString GlobalMessageReceiverInterface::get_name() const{
		return _name;
}

