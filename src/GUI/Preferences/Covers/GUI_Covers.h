#ifndef GUI_COVERS_H
#define GUI_COVERS_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

namespace Ui { class GUI_Covers; }

class GUI_Covers :
		public PreferenceWidgetInterface
{

public:
	GUI_Covers(QWidget* parent=nullptr);
	virtual ~GUI_Covers();

	void commit() override;
	void revert() override;

	QString get_action_name() const override;

protected:
	void init_ui() override;
	void retranslate_ui() override;

private:
	Ui::GUI_Covers* ui=nullptr;
};


#endif // GUI_COVERS_H
