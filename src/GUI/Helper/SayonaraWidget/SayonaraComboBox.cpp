#include "SayonaraComboBox.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/Delegates/StyledItemDelegate.h"
#include "Helper/Settings/Settings.h"

#include <QAbstractItemView>
#include <QEvent>

SayonaraComboBox::SayonaraComboBox(QWidget* parent) :
    SayonaraWidgetTemplate<QComboBox>(parent)
{
	this->setItemDelegate(new ComboBoxDelegate(this));

	REGISTER_LISTENER(Set::Player_Style, style_changed);
}

SayonaraComboBox::~SayonaraComboBox() {}

void SayonaraComboBox::style_changed()
{
	bool is_dark = (_settings->get(Set::Player_Style) == 1);
	QAbstractItemView* view = this->view();
	if(!view){
		return;
	}

	QWidget* parent_widget = view->parentWidget();
	if(!parent_widget){
		return;
	}

	if(is_dark) {
		parent_widget->setStyleSheet("background: #3c3c3c; margin: -2px; padding: 0px; border: none;");
	}

	else {
		parent_widget->setStyleSheet("");
	}
}

void SayonaraComboBox::changeEvent(QEvent* event)
{
	SayonaraWidgetTemplate<QComboBox>::changeEvent(event);

	if(event->type() == QEvent::StyleChange){

		QFontMetrics f(this->font());
		int h = (f.height() * 3) / 2;
		this->setIconSize(QSize(h, h));
	}

	update();
	repaint();
}

