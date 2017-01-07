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
