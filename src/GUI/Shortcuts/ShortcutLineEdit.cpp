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

	bool meta_pressed=false;

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
}
