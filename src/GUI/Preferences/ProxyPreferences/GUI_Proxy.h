#ifndef GUI_PROXY_H
#define GUI_PROXY_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"
#include "GUI/Helper/GUIClass.h"

UI_FWD(GUI_Proxy)

class GUI_Proxy :
        public PreferenceWidgetInterface
{
	Q_OBJECT
	UI_CLASS(GUI_Proxy)

public:
    explicit GUI_Proxy(QWidget *parent = 0);
    ~GUI_Proxy();

    // PreferenceInterface interface
protected:
    void init_ui() override;
    void retranslate_ui() override;

public:
    QString get_action_name() const override;
    void commit() override;
    void revert() override;

private slots:
    void active_toggled(bool active);
};



#endif // GUI_PROXY_H
