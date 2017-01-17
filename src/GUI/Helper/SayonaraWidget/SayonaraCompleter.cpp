#include "SayonaraCompleter.h"
#include "GUI/Helper/Style/Style.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"

#include <QStringList>
#include <QAbstractItemView>

SayonaraCompleter::SayonaraCompleter(const QStringList& lst, QObject* parent) :
	QCompleter(lst, parent)
{
	setCaseSensitivity(Qt::CaseInsensitive);
	setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	popup()->setItemDelegate(new ComboBoxDelegate(this));
	popup()->setStyleSheet(Style::get_current_style());
}

SayonaraCompleter::~SayonaraCompleter() {}

void SayonaraCompleter::set_stringlist(const QStringList& lst)
{
	QAbstractItemModel* model = this->model();
	if(!model){
		return;
	}

	model->removeRows(0, this->model()->rowCount());
	model->insertRows(0, lst.size());

	int idx=0;
	for(const QString& str : lst){
		model->setData(model->index(idx++, 0), str);
	}
}

