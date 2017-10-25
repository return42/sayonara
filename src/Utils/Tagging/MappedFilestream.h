#ifndef MAPPEDFILESTREAM_H
#define MAPPEDFILESTREAM_H

#include <taglib/tiostream.h>

#include "Utils/Pimpl.h"

namespace TagLib
{
	class ByteVector;
}

namespace Tagging
{
	class MappedFileStream : TagLib::IOStream
	{
		PIMPL(MappedFileStream)

		public:
			MappedFileStream(const TagLib::FileName& filename);
			~MappedFileStream();

			TagLib::FileName name () const override;
			TagLib::ByteVector readBlock(unsigned long length) override;
			void writeBlock(const TagLib::ByteVector &data) override;
			void insert(const TagLib::ByteVector &data, unsigned long start=0, unsigned long replace=0) override;
			void removeBlock(unsigned long start=0, unsigned long length=0) override;
			bool readOnly() const override;
			bool isOpen() const override;
			void seek(long offset, TagLib::IOStream::Position p=Beginning) override;
			void clear() override;
			long tell() const override;
			long length () override;
			void truncate (long length) override;
	};
}


typedef struct stat FileStats;

#endif // MAPPEDFILESTREAM_H
