#ifndef SAYONARACOMBOBOX_H
#define SAYONARACOMBOBOX_H

#include "GUI/Utils/Widgets/Widget.h"
#include <QComboBox>

namespace Gui
{
	class ComboBox :
			public Gui::WidgetTemplate<QComboBox>
	{
		Q_OBJECT

	public:
		explicit ComboBox(QWidget* parent=nullptr);
		virtual ~ComboBox();

	protected:
		void changeEvent(QEvent* e) override;

	};
}

#endif // SAYONARACOMBOBOX_H
