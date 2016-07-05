#ifndef SAYONARASMALLPROGRESSBAR_H
#define SAYONARASMALLPROGRESSBAR_H

#include <QProgressBar>
#include <QWidget>

class SayonaraSmallProgressBar :
        public QProgressBar
{
public:
        SayonaraSmallProgressBar(QWidget* parent=nullptr) :
	    QProgressBar(parent)
	{

	}
};

#endif // SAYONARASMALLPROGRESSBAR_H
