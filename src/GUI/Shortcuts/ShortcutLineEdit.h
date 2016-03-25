#ifndef SHORTCUTLINEEDIT_H
#define SHORTCUTLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class ShortcutLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	ShortcutLineEdit(QWidget* parent=nullptr);

private:
	void keyPressEvent(QKeyEvent* e);
};

#endif // SHORTCUTLineEdit_H
