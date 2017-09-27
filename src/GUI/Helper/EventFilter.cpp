#include "EventFilter.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>

KeyPressFilter::KeyPressFilter(QObject *parent) :
    QObject(parent)
{}


bool KeyPressFilter::eventFilter(QObject *o, QEvent *e)
{
    e->accept();

    if(e->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(e);
        if(ke->key() == Qt::Key_Escape) {
            emit sig_esc_pressed();
        }
    }

    return QObject::eventFilter(o, e);
}



ContextMenuFilter::ContextMenuFilter(QObject* parent) :
    QObject(parent)
{}

bool ContextMenuFilter::eventFilter(QObject *o, QEvent *e)
{
    e->accept();

    if(e->type() == QEvent::ContextMenu)
    {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(e);

        emit sig_context_menu(cme->globalPos(), nullptr);
    }

    return QObject::eventFilter(o, e);
}


