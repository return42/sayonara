#ifndef MimeDataUtils_H
#define MimeDataUtils_H

class QMimeData;
class MetaDataList;
class CustomMimeData;
class QStringList;
class QString;

namespace Gui
{
	namespace Util
	{
		namespace MimeData
		{
			MetaDataList get_metadata(const QMimeData* data);

			QStringList get_playlists(const QMimeData* data);

			const CustomMimeData* cast(const QMimeData* data);

			bool is_inner_drag_drop(const QMimeData* data);
			void set_inner_drag_drop(QMimeData* data);

			void set_cover_url(QMimeData* data, const QString& url);
			QString cover_url(const QMimeData* data);
		}
	}
}

#endif // MimeDataUtils_H
