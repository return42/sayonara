#ifndef LINEINPUTDIALOG_H
#define LINEINPUTDIALOG_H

#include <QInputDialog>

class LineInputDialog :
	public QInputDialog
{

public:
	LineInputDialog(const QString& title, const QString& label, const QString& initial_value, QWidget* parent);
	LineInputDialog(const QString& title, const QString& label, QWidget* parent);
	~LineInputDialog();

private slots:
	void textValueChanged(const QString& text);
};

#endif // LINEINPUTDIALOG_H
