#include "Frames/Cover.h"

ID3v2Frame::Cover::Cover(){
	description = "Cover by Sayonara Player";
}

ID3v2Frame::Cover::Cover(const QString& mime_type_, const QByteArray& image_data_) : 
	ID3v2Frame::Cover::Cover()
{
	mime_type = mime_type_;
	image_data = image_data_;
}

ID3v2Frame::CoverFrame::CoverFrame(TagLib::FileRef* file_ref) :
	AbstractFrame<Cover, TagLib::ID3v2::AttachedPictureFrame>(file_ref, "APIC")
{

}

ID3v2Frame::CoverFrame::~CoverFrame(){

}

void  ID3v2Frame::CoverFrame::map_model_to_frame(){

	TagLib::String description = TagLib::String("Cover by Sayonara Player");
	TagLib::String::Type encoding = TagLib::String::Latin1;
	TagLib::String mime_type = TagLib::String(_data_model.mime_type.toLatin1().constData());
	TagLib::ID3v2::AttachedPictureFrame::Type type = TagLib::ID3v2::AttachedPictureFrame::FrontCover;

	TagLib::ByteVector taglib_image_data;
	const QByteArray& image_data = _data_model.image_data;

	taglib_image_data.setData(image_data.data(), image_data.size());

	TagLib::ByteVector vec, vec_header;
	vec_header = TagLib::ByteVector("APIC", 4);

	_frame->setDescription(description);
	_frame->setTextEncoding(encoding);
	_frame->setMimeType(mime_type);
	_frame->setType(type);
	_frame->setPicture(taglib_image_data);

	vec = _frame->render();

	if( !vec.startsWith(vec_header) ){
		vec = vec_header + vec;
	}

	_frame->setData(vec);
}

void ID3v2Frame::CoverFrame::map_frame_to_model(){


	TagLib::ByteVector taglib_image_data = _frame->picture();
	TagLib::String mime_type = _frame->mimeType();

	_data_model.image_data = QByteArray(taglib_image_data.data(), taglib_image_data.size());
	_data_model.mime_type = QString::fromLatin1(mime_type.toCString(), mime_type.length());
}

TagLib::ID3v2::Frame* ID3v2Frame::CoverFrame::create_id3v2_frame(){

	return new TagLib::ID3v2::AttachedPictureFrame(TagLib::ByteVector());
}



