#ifndef GUI_SOMAFM_H
#define GUI_SOMAFM_H

#include <QWidget>
#include "SomaFMLibrary.h"
#include "SomaFMStation.h"

#include "3rdParty/SomaFM/ui_GUI_SomaFM.h"
#include "Components/CoverLookup/CoverLocation.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

#include <QItemSelection>


class SomaFMStationModel;

class GUI_SomaFM :
		public SayonaraWidget,
		protected Ui::GUI_SomaFM

{
	Q_OBJECT

private:
	SomaFMLibrary*	_library=nullptr;

public:
	explicit GUI_SomaFM(QWidget *parent = 0);
	~GUI_SomaFM();

	QComboBox*	get_libchooser() const;


private slots:
	void stations_loaded(const QList<SomaFMStation>& stations);
	void station_changed(const SomaFMStation& station);

	void station_clicked(const QModelIndex& idx);
	void station_index_changed(const QModelIndex& idx);
	void playlist_double_clicked(const QModelIndex& idx);
	void cover_found(const CoverLocation& cover_location);

	void selection_changed(const QModelIndexList& selected);

private:
	SomaFMStation get_station(int row) const;
};

#endif // GUI_SOMAFM_H
