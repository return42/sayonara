/* SayonaraLoadingBar.h */

#ifndef SAYONARALOADINGBAR_H
#define SAYONARALOADINGBAR_H

#include <QProgressBar>
#include <QShowEvent>

class SayonaraLoadingBar : public QProgressBar
{

private:
	QWidget*	_parent=nullptr;
	int			_fixed_height;

public:
	SayonaraLoadingBar(QWidget* parent);

protected:
	void showEvent(QShowEvent* e);
};



#endif // SAYONARALOADINGBAR_H
