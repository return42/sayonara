#ifndef SAYONARACOMBOBOX_H
#define SAYONARACOMBOBOX_H

#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"
#include <QComboBox>

class SayonaraComboBox :
        public SayonaraWidgetTemplate<QComboBox>
{
	Q_OBJECT

public:
	explicit SayonaraComboBox(QWidget* parent=nullptr);
	virtual ~SayonaraComboBox();

protected:
	void changeEvent(QEvent* e) override;
};

#endif // SAYONARACOMBOBOX_H
