#ifndef VERTICALHEADER_H
#define VERTICALHEADER_H

#include <QHeaderView>

class VerticalHeader :
        public QHeaderView
{

public:
	QSize sizeHint() const override;
};

#endif // VERTICALHEADER_H
