#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include "Helper/Pimpl.h"

class QPoint;
class QPixmap;
class QMimeData;
class QWidget;
class QDrag;

class Draggable
{

public:
	explicit Draggable(QWidget* parent);
	virtual ~Draggable();

	enum class ReleaseReason : char
	{
		Dropped,
		Destroyed
	};

private:
	PIMPL(Draggable)

protected:
	virtual void	drag_pressed(const QPoint& p) final;
	virtual QDrag*	drag_moving(const QPoint& p) final;
	virtual void	drag_released(ReleaseReason reason);

	virtual QMimeData*	get_mime_data() const=0;
	virtual QPixmap		get_pixmap() const;
};

#endif // DRAGGABLE_H
