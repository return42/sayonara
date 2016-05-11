#ifndef GUI_SOMAFM_H
#define GUI_SOMAFM_H

#include <QWidget>
#include "SomaFMLibrary.h"
#include "3rdParty/SomaFM/ui_GUI_SomaFM.h"
#include "Components/CoverLookup/CoverLocation.h"
#include "Helper/MetaData/MetaDataList.h"

#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"


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
	void stations_loaded(const QStringList& stations);
	void station_loaded(const SomaFMStation& station);

	void station_index_changed(const QModelIndex& idx);
	void playlist_double_clicked(const QModelIndex& idx);
	void cover_found(const CoverLocation& cover_location);
};

#endif // GUI_SOMAFM_H
