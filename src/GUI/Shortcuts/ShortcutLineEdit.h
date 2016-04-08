#ifndef SHORTCUTLINEEDIT_H
#define SHORTCUTLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>

class ShortcutLineEdit : public QLineEdit
{
	Q_OBJECT

signals:
	void sig_sequence_entered();

public:
	ShortcutLineEdit(QWidget* parent=nullptr);

	QList<QKeySequence> get_sequences() const;

private:
	void keyPressEvent(QKeyEvent* e);
};

#endif // SHORTCUTLineEdit_H
