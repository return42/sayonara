#include "Draggable.h"
#include <QPoint>
#include <QMimeData>
#include <QPixmap>
#include <QDrag>
#include <QPainter>
#include <QApplication>
#include <QWidget>
#include <QUrl>

#include "GUI/Helper/GUI_Helper.h"
#include "Helper/FileHelper.h"

struct Draggable::Private
{
	QPoint		start_drag_pos;
	bool		dragging=false;
	QWidget*	parent=nullptr;
	QDrag*		drag=nullptr;

	Private(){
		start_drag_pos = QPoint();
		dragging = false;
	}

	QStringList get_strings(const QMimeData* data)
	{
		QStringList ret;
		QStringList playlist_files;
		QStringList directories;
		QStringList sound_files;

		QList<QUrl> urls = data->urls();
		for(const QUrl& url : urls)
		{
			QString filename = url.toLocalFile();
			if(Helper::File::is_playlistfile(filename)){
				playlist_files << filename;
			}

			else if(Helper::File::is_soundfile(filename)){
				sound_files << filename;
			}

			else if(Helper::File::is_dir(filename)){
				directories << filename;
			}
		}



	}
};


Draggable::Draggable(QWidget* parent)
{
	_m = Pimpl::make<Draggable::Private>();
	_m->parent = parent;
}

Draggable::~Draggable() {}

void Draggable::drag_pressed(const QPoint& p)
{
	_m->dragging = false;
	_m->start_drag_pos = p;
}


QDrag* Draggable::drag_moving(const QPoint& p)
{
	int distance = (p - _m->start_drag_pos).manhattanLength();

	if( distance < QApplication::startDragDistance())
	{
		return _m->drag;
	}

	if(_m->dragging)
	{
		return _m->drag;
	}

	_m->dragging = true;
	_m->start_drag_pos = QPoint();
	_m->drag = new QDrag(_m->parent);

	QMimeData* data = get_mime_data();
	if(data == nullptr)
	{
		return _m->drag;
	}


	QString text = QString("%1 tracks").arg(data->urls().size());

	const int pm_height = 24;
	const int pm_width = 100;
	const int logo_height = 16;
	const QSize logo_size(logo_height, logo_height);
	const int right_offset = 4;

	QPixmap cover = get_pixmap();
	if(cover.isNull()){
		cover = GUI::get_pixmap("logo.png", logo_size, true);
	}

	QPixmap pm(pm_width, pm_height);
	QPainter painter(&pm);

	painter.fillRect(pm.rect(), QColor(64, 64, 64));
	painter.setPen(QColor("#424e72"));
	painter.drawRect(0, 0, pm_width - 1, pm_height - 1);
	painter.drawPixmap(right_offset, (pm_height - logo_height) / 2, logo_height, logo_height, cover);
	painter.setPen(QColor(255, 255, 255));
	painter.translate(logo_height + 15, (pm_height - 10) / 2 + 10);
	painter.drawText(0, 0, text);

	_m->drag->setMimeData(data);
	_m->drag->setPixmap(pm);
	_m->drag->exec(Qt::CopyAction);

	return _m->drag;
}


void Draggable::drag_released(Draggable::ReleaseReason reason)
{
	if(reason == Draggable::ReleaseReason::Destroyed)
	{
		_m->drag = nullptr;
	}

	else if(_m->drag){
		delete _m->drag; _m->drag = nullptr;
	}

	_m->dragging = false;
	_m->start_drag_pos = QPoint();
}


QPixmap Draggable::get_pixmap() const
{
	return QPixmap();
}
