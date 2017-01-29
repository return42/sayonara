#include "Cover.h"

Models::Cover::Cover()
{
	description = "Cover by Sayonara Player";
}

Models::Cover::Cover(const QString& mime_type_, const QByteArray& image_data_) :
	Models::Cover::Cover()
{
	mime_type = mime_type_;
	image_data = image_data_;
}

Models::Cover::MimeType Models::Cover::get_mime_type() const
{
	if(mime_type.contains("jpeg", Qt::CaseInsensitive)){
		return Models::Cover::MimeType::JPEG;
	}

	else if(mime_type.contains("png", Qt::CaseInsensitive)){
		return Models::Cover::MimeType::PNG;
	}

	return Models::Cover::MimeType::Unsupported;
}
