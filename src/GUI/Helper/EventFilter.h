#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>

class QEvent;
class QAction;
class KeyPressFilter :
        public QObject
{
    Q_OBJECT

public:
    explicit KeyPressFilter(QObject* parent=nullptr);

signals:
    void sig_esc_pressed();

protected:
    bool eventFilter(QObject* o , QEvent* e);
};


class ContextMenuFilter :
        public QObject
{
    Q_OBJECT

public:
    explicit ContextMenuFilter(QObject* parent=nullptr);

signals:
    void sig_context_menu(const QPoint& p, QAction* a);

protected:
    bool eventFilter(QObject* o , QEvent* e);
};

#endif // EVENTFILTER_H
