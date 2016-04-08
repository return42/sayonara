#include "ShortcutLineEdit.h"
#include "Helper/Logger/Logger.h"

bool is_modifier(int key){
	return (key == Qt::Key_Control
			|| key == Qt::Key_Alt
			|| key == Qt::Key_AltGr
			|| key == Qt::Key_Shift
			|| key == Qt::Key_Meta
			);
}


ShortcutLineEdit::ShortcutLineEdit(QWidget*parent) :
	QLineEdit(parent)
{
}

QList<QKeySequence> ShortcutLineEdit::get_sequences() const
{
	QStringList lst = this->text().split(",");
	QList<QKeySequence> sequences;
	for(const QString& str : lst){
		if(str.isEmpty()){
			continue;
		}

		sequences << QKeySequence::fromString(str, QKeySequence::NativeText);
	}

	return sequences;
}

void ShortcutLineEdit::keyPressEvent(QKeyEvent* e)
{
	int key = e->key();

	if(key == Qt::Key_Escape && e->modifiers() == Qt::NoModifier){
		this->setText("");
		return;
	}

	if(e->modifiers() == Qt::NoModifier){
		return;
	}

	if(is_modifier(key)){
		key = 0;
	}

	if(e->modifiers() & Qt::ControlModifier){
		key |= Qt::CTRL;
	}

	if(e->modifiers() & Qt::ShiftModifier){
		key |= Qt::SHIFT;
	}

	if(e->modifiers() & Qt::MetaModifier){
		key |= Qt::META;
	}

	if(e->modifiers() & Qt::AltModifier){
		key |= Qt::ALT;
	}

	if(e->modifiers() & Qt::KeypadModifier){
		key |= Qt::KeypadModifier;
	}

	QKeySequence ks(key);
	this->setText(ks.toString(QKeySequence::NativeText));

	emit sig_sequence_entered();
}
