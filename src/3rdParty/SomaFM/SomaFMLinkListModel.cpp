#include "SomaFMLinkListModel.h"
#include <QUrl>
#include <QMimeData>
#include "Helper/MetaData/MetaDataList.h"
#include "GUI/Helper/CustomMimeData.h"

SomaFMLinkListModel::SomaFMLinkListModel(QObject *parent) :
	QStringListModel(parent)
{
}


QStringList SomaFMLinkListModel::mimeTypes() const
{
	QStringList mime_types;
	mime_types << "text/uri-list";
	return mime_types;
}

QMimeData* SomaFMLinkListModel::mimeData(const QModelIndexList& indexes) const
{
	CustomMimeData* data = new CustomMimeData();

	MetaDataList v_md;
	for(const QModelIndex& idx : indexes){

		int row = idx.row();
		v_md << _v_md[row];
	}

	data->setText("tracks");
	data->setMetaData(v_md);

	return data;
}

void SomaFMLinkListModel::set_metadata(const MetaDataList &v_md){

	QStringList urls;
	for(const MetaData& md : v_md){
		urls << md.filepath();
	}

	this->setStringList(urls);

	_v_md = v_md;
}

