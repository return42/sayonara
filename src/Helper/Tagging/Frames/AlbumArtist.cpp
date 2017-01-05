#include "AlbumArtist.h"
#include <taglib/textidentificationframe.h>
#include "Helper/Logger/Logger.h"

ID3v2Frame::AlbumArtistFrame::AlbumArtistFrame(TagLib::FileRef* file_ref) :
	AbstractFrame<QString, TagLib::ID3v2::TextIdentificationFrame>(file_ref, "TPE2")
{

}

ID3v2Frame::AlbumArtistFrame::~AlbumArtistFrame() {}

TagLib::ID3v2::Frame* ID3v2Frame::AlbumArtistFrame::create_id3v2_frame()
{
	return new TagLib::ID3v2::TextIdentificationFrame(TagLib::ByteVector());
}

void ID3v2Frame::AlbumArtistFrame::map_model_to_frame()
{
	QByteArray byte_arr_header, byte_arr_body;
	TagLib::ByteVector data;

	byte_arr_body = QByteArray((const char*) _data_model.utf16(), _data_model.size() * 2);
	int size = byte_arr_body.size() + 3;

	byte_arr_header.push_back("TPE2");
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((quint8) (size >> 7) & (0x7F));
	byte_arr_header.push_back((quint8) size & (0x7F));
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x00);
	byte_arr_header.push_back((char) 0x01);
	byte_arr_header.push_back((char) 0xFF);
	byte_arr_header.push_back((char) 0xFE);

	byte_arr_header.push_back(byte_arr_body);

	data.setData(byte_arr_header.data(), byte_arr_header.size());

	_frame->setData(data);
}

void ID3v2Frame::AlbumArtistFrame::map_frame_to_model()
{
	TagLib::String tag_str = _frame->toString();
	QString str = QString::fromUtf8( tag_str.toCString(true) );
	_data_model = str;
}
