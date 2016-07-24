#ifndef SOMAFMPLAYLISTMODEL_H
#define SOMAFMPLAYLISTMODEL_H

#include <QStringListModel>
#include <QMimeData>
#include <QUrl>

#include "3rdParty/SomaFM/SomaFMStation.h"

class SomaFMStation;
class SomaFMPlaylistModel : public QStringListModel
{
    Q_OBJECT

private:
	SomaFMStation _station;

public:
    SomaFMPlaylistModel(QObject* parent=nullptr);
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

    void setStation(const SomaFMStation& station);



};


#endif // SOMAFMPLAYLISTMODEL_H
