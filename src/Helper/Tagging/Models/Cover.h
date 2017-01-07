#ifndef COVER_MODELS_H_
#define COVER_MODELS_H_

#include <QString>

namespace Models
{
    class Cover
    {
		private:
			unsigned char text_encoding;
			unsigned char picture_type;
			QString description;

		public:
			QString mime_type;
			QByteArray image_data;

			Cover();
			Cover(const QString& mime_type, const QByteArray& image_data);
    };
}

#endif // COVER_H
