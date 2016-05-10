#ifndef SOMAFMLINKLISTMODEL_H
#define SOMAFMLINKLISTMODEL_H

#include <QStringListModel>
#include <QMimeData>
#include "SomaFMLibrary.h"
#include "Helper/MetaData/MetaDataList.h"

class SomaFMLinkListModel : public QStringListModel
{
	Q_OBJECT
public:
	explicit SomaFMLinkListModel(QObject *parent = 0);

public:
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const override;

	void set_metadata(const MetaDataList& v_md);

private:
	MetaDataList _v_md;

};

#endif // SOMAFMLINKLISTMODEL_H
