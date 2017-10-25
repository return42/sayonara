#include "MappedFilestream.h"
#include "Utils/Logger/Logger.h"

#include <taglib/fileref.h>
#include <taglib/tfile.h>
#include <taglib/tfilestream.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstdlib>
#include <limits>
#include <cmath>
#include <unistd.h>


struct Tagging::MappedFileStream::Private
{
	TagLib::FileName filename;
	bool isOpen;
	long length;
	long cur_read_offset;
	char* data = nullptr;
	int fd;

	Private(const TagLib::FileName& filename) :
		filename(filename),
		isOpen(false),
		length(0),
		cur_read_offset(0),
		fd(-1)
	{
		FileStats stats;
		stat(filename, &stats);
		length = stats.st_size;

		fd = open(filename, O_RDONLY, 0);
		data = (char*) mmap(NULL, length, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

		if(data != MAP_FAILED)
		{
			isOpen = (length > 0);
		} else {
			length = 0;
		}
	}

	~Private()
	{
		munmap(data, length);
		close(fd);
	}
};

Tagging::MappedFileStream::MappedFileStream(const TagLib::FileName& filename) :
	TagLib::IOStream()
{
	m = Pimpl::make<Private>(filename);
}

Tagging::MappedFileStream::~MappedFileStream() {}


TagLib::FileName Tagging::MappedFileStream::name() const
{
	return m->filename;
}

TagLib::ByteVector Tagging::MappedFileStream::readBlock(unsigned long length)
{
	if(m->cur_read_offset < 0)
	{
		sp_log(Log::Warning, this) << "  ReadBlock: Read pointer negative!";
		return TagLib::ByteVector();
	}

	length = std::min((unsigned long) (m->length - m->cur_read_offset), length);
	if(length <= 0) {
		sp_log(Log::Warning, this) << "  ReadBlock: Length <= 0: " << length;
		return TagLib::ByteVector();
	}

	if(length > std::numeric_limits<unsigned int>::max()) {
		sp_log(Log::Warning, this) << "  ReadBlock: Length too big " << length;
		return TagLib::ByteVector();
	}

	return TagLib::ByteVector(m->data + m->cur_read_offset, length);
}



bool Tagging::MappedFileStream::readOnly() const
{
	return true;
}

bool Tagging::MappedFileStream::isOpen() const
{
	return m->isOpen;
}

void Tagging::MappedFileStream::seek(long offset, TagLib::IOStream::Position p)
{
	long old_read_offset = m->cur_read_offset;

	switch(p)
	{
		case TagLib::IOStream::Position::Beginning:
		{
			m->cur_read_offset = (offset >= m->length || offset < 0) ? -1 : offset;
		} break;

		case TagLib::IOStream::Position::Current:
		{
			long new_offset = offset + m->cur_read_offset;
			m->cur_read_offset = (new_offset >= m->length || new_offset < 0) ? -1 : new_offset;
		} break;

		case TagLib::IOStream::Position::End:
		{
			long new_offset = m->length - 1 - std::abs(offset);
			m->cur_read_offset = (new_offset >= m->length || new_offset < 0) ? -1 : new_offset;
		} break;
	}

	if(m->cur_read_offset < 0)
	{
		sp_log(Log::Warning, this) << "Read pointer out of scope: ";
		sp_log(Log::Warning, this) << "  Read pointer before: " << old_read_offset;
		sp_log(Log::Warning, this) << "  Offset: " << offset;
		sp_log(Log::Warning, this) << "  Position: " << (int) p;
	}
}

void Tagging::MappedFileStream::clear()
{
	TagLib::IOStream::clear();
}

long Tagging::MappedFileStream::tell() const
{
	return m->cur_read_offset;
}

long Tagging::MappedFileStream::length ()
{
	return m->length;
}


void Tagging::MappedFileStream::writeBlock(const TagLib::ByteVector &data)
{
	Q_UNUSED(data)
	sp_log(Log::Warning, this) << __FUNCTION__ << " not implemented";
}

void Tagging::MappedFileStream::insert(const TagLib::ByteVector &data, unsigned long start, unsigned long replace)
{
	Q_UNUSED(data)
	Q_UNUSED(start)
	Q_UNUSED(replace)

	sp_log(Log::Warning, this) << __FUNCTION__ << " not implemented";
}

void Tagging::MappedFileStream::removeBlock(unsigned long start, unsigned long length)
{
	Q_UNUSED(start)
	Q_UNUSED(length)

	sp_log(Log::Warning, this) << __FUNCTION__ << " not implemented";
}


void Tagging::MappedFileStream::truncate(long length)
{
	Q_UNUSED(length)
	sp_log(Log::Warning, this) << __FUNCTION__ << " not implemented";
}
