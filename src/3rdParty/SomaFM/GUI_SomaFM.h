#ifndef GUI_SOMAFM_H
#define GUI_SOMAFM_H

#include <QWidget>
#include "ui_GUI_SomaFM.h"
#include "Components/CoverLookup/CoverLocation.h"
#include "Helper/MetaData/MetaDataList.h"
#include "SomaFMLibrary.h"

#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"
#include "Interfaces/LibraryInterface/LibraryContainer/LibraryContainer.h"


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
	void cover_found(const CoverLocation& cover_location);
};



class SomaFMLibraryContainer :
	public LibraryContainerInterface
{

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.sayonara-player.somafm_library")
	Q_INTERFACES(LibraryContainerInterface)


private:
	GUI_SomaFM*	ui=nullptr;


public:

	SomaFMLibraryContainer(QObject* parent=nullptr);

	// override from LibraryViewInterface
	QString			get_name() const override;
	QString			get_display_name() const override;
	QIcon			get_icon() const override;
	QWidget*		get_ui() const override;
	QComboBox*		get_libchooser() override;
	QMenu*			get_menu() override;
	void			init_ui() override;

};


#endif // GUI_SOMAFM_H
