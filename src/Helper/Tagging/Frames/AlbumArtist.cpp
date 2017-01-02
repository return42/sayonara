#include "AlbumArtist.h"
#include <taglib/textidentificationframe.h>

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
	TagLib::String str(_data_model.toUtf8().data());
	_frame->setText(str);
}

void ID3v2Frame::AlbumArtistFrame::map_frame_to_model()
{
	TagLib::String tag_str = _frame->toString();
	QString str = QString::fromUtf8( tag_str.toCString(true) );
	_data_model = str;
}
