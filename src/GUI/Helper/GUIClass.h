#ifndef GUICLASS_H
#define GUICLASS_H

#define UI_FWD(x) namespace Ui { class x ; }
#define UI_CLASS(x) private: \
	Ui:: x *ui=nullptr;

#endif // GUICLASS_H
