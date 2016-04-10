#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QByteArray>

namespace Compressor
{
	QByteArray compress(const QByteArray& arr);
	QByteArray decompress(const QByteArray& arr);
}

#endif // COMPRESSOR_H
