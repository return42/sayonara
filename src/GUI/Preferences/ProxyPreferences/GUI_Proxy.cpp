#include "GUI_Proxy.h"
#include "ui_GUI_Proxy.h"
#include "Utils/Settings/Settings.h"

GUI_Proxy::GUI_Proxy(QWidget *parent) :
    PreferenceWidgetInterface(parent)
{}

GUI_Proxy::~GUI_Proxy()
{
    if(ui)
    {
        delete ui; ui=nullptr;
    }
}

void GUI_Proxy::init_ui()
{
    setup_parent(this, &ui);

    connect(ui->cb_active, &QCheckBox::toggled, this, &GUI_Proxy::active_toggled);

    revert();
}

void GUI_Proxy::retranslate_ui()
{
    ui->retranslateUi(this);
}

QString GUI_Proxy::get_action_name() const
{
    return tr("Proxy");
}

void GUI_Proxy::commit()
{
    _settings->set(Set::Proxy_Active, ui->cb_active->isChecked());
    _settings->set(Set::Proxy_Username, ui->le_username->text());

    _settings->set(Set::Proxy_Hostname, ui->le_host->text());
    _settings->set(Set::Proxy_Port, ui->sb_port->value());
    _settings->set(Set::Proxy_SavePw, ui->cb_save_pw->isChecked());

    if(ui->cb_save_pw->isChecked()) {
        _settings->set(Set::Proxy_Password, ui->le_password->text());
    }
    else {
        _settings->set(Set::Proxy_Password, QString());
    }
}

void GUI_Proxy::revert()
{
    bool active = _settings->get(Set::Proxy_Active);

    ui->cb_active->setChecked(active);

    ui->le_host->setText(_settings->get(Set::Proxy_Hostname));
    ui->sb_port->setValue(_settings->get(Set::Proxy_Port));
    ui->le_username->setText(_settings->get(Set::Proxy_Username));
    ui->le_password->setText(_settings->get(Set::Proxy_Password));
    ui->cb_save_pw->setChecked(_settings->get(Set::Proxy_SavePw));

    active_toggled(active);
}

void GUI_Proxy::active_toggled(bool active)
{
    ui->le_host->setEnabled(active);
    ui->le_password->setEnabled(active);
    ui->sb_port->setEnabled(active);
    ui->le_username->setEnabled(active);
    ui->cb_save_pw->setEnabled(active);
}
