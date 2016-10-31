#ifndef GUI_DATESEARCHCONFIG_H
#define GUI_DATESEARCHCONFIG_H

#include "GUI/Helper/SayonaraWidget/SayonaraDialog.h"
#include "Helper/Pimpl.h"

#include <QShowEvent>
#include <QString>

namespace Library { class DateFilter; }
namespace Ui{ class GUI_DateSearchConfig; }

class GUI_DateSearchConfig :
		public SayonaraDialog
{

public:
	explicit GUI_DateSearchConfig(QWidget* parent=nullptr);
	~GUI_DateSearchConfig();

	void set_filter(const Library::DateFilter& filter);
	Library::DateFilter get_edited_filter() const;


protected slots:
	void language_changed() override;

	void ok_clicked();
	void save_as_clicked();
	void cancel_clicked();


private:
	PIMPL(GUI_DateSearchConfig)

	Ui::GUI_DateSearchConfig*	 ui=nullptr;

private:
	void commit();
};


#endif // GUI_DATESEARCHCONFIG_H
