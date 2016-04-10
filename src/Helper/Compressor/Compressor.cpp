#include "Compressor.h"

#include <iostream>
#include "zlib.h"



QByteArray Compressor::compress(const QByteArray &arr){

	QByteArray target(arr.size(), 0);

	z_stream comp_stream;
	comp_stream.zalloc = Z_NULL;
	comp_stream.zfree = Z_NULL;
	comp_stream.opaque = Z_NULL;

	comp_stream.avail_in = static_cast<uInt>(arr.size() + 1);
	comp_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(arr.data()));
	comp_stream.avail_out = static_cast<uInt>(target.size());
	comp_stream.next_out = reinterpret_cast<Bytef*>(const_cast<char*>(target.data()));

	deflateInit(&comp_stream, Z_BEST_COMPRESSION);
	deflate(&comp_stream, Z_FINISH);
	deflateEnd(&comp_stream);

	target.resize(comp_stream.total_out);
	return target;

}

QByteArray Compressor::decompress(const QByteArray& arr){

	QByteArray target(arr.size() * 10, 0);

	z_stream decomp_stream;
	decomp_stream.zalloc = Z_NULL;
	decomp_stream.zfree = Z_NULL;
	decomp_stream.opaque = Z_NULL;

	decomp_stream.avail_in = static_cast<uInt>(arr.size() + 1);
	decomp_stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(arr.data()));
	decomp_stream.avail_out = static_cast<uInt>(target.size());
	decomp_stream.next_out = reinterpret_cast<Bytef*>(const_cast<char*>(target.data()));

	inflateInit(&decomp_stream);
	inflate(&decomp_stream, Z_NO_FLUSH);
	inflateEnd(&decomp_stream);

	target.resize(decomp_stream.total_out);
	return target;
}
