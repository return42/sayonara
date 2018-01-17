#ifndef HEADERMODEL_H
#define HEADERMODEL_H

#include <QAbstractItemModel>

namespace Playlist
{
	class HeaderModel :
			public QAbstractItemModel
	{
		Q_OBJECT

		public:
			HeaderModel(QObject* parent=nullptr);
			~HeaderModel();
	};
}

#endif // HEADERMODEL_H
