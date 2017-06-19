#include "SayonaraComboBox.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/Delegates/StyledItemDelegate.h"
#include "Helper/Settings/Settings.h"

#include <QAbstractItemView>
#include <QEvent>

SayonaraComboBox::SayonaraComboBox(QWidget* parent) :
	SayonaraWidgetTemplate<QComboBox>(nullptr)
{
	Q_UNUSED(parent)

	this->setItemDelegate(new ComboBoxDelegate(this));
}

SayonaraComboBox::~SayonaraComboBox() {}



void SayonaraComboBox::changeEvent(QEvent* event)
{
	SayonaraWidgetTemplate<QComboBox>::changeEvent(event);

	if(event->type() == QEvent::StyleChange){

		QFontMetrics f(this->font());
		int h = f.height();
		h = std::max(h, 16);

		this->setIconSize(QSize(h, h));
	}

	update();
	repaint();
}

