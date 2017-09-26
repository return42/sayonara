#ifndef GUI_PROXY_H
#define GUI_PROXY_H

#include "Interfaces/PreferenceDialog/PreferenceWidgetInterface.h"

namespace Ui {
    class GUI_Proxy;
}

class GUI_Proxy :
        public PreferenceWidgetInterface
{
    Q_OBJECT

public:
    explicit GUI_Proxy(QWidget *parent = 0);
    ~GUI_Proxy();

private:
    Ui::GUI_Proxy *ui=nullptr;

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
